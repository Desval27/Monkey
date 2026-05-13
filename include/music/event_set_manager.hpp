#pragma once

#include <algorithm>
#include <type_traits>

#include <music/event_set.hpp>
#include <music/persona.hpp>

namespace music {

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam EVENT_TYPE
/// @tparam MAX_DEGREES
/// @tparam SCALE_DEGREES
/// @tparam MAX_EVENTS
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS,
         typename EVENT_TYPE = NoteEventSet<MAX_EVENTS>>
class EventSetManager
{
  /////////////////////////////////////////////////////////////////////////////
  // Aliases because I'm lazy
  /////////////////////////////////////////////////////////////////////////////
  using TSetup = Setup<MAX_DEGREES, SCALE_DEGREES>;
  using TNoteEvent = NoteEvent;
  using TNoteEventSet = NoteEventSet<MAX_EVENTS>;
  using TChordEvent = ChordEvent<MAX_DEGREES, SCALE_DEGREES>;
  using TChordEventSet = ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
  using TEvent = typename EVENT_TYPE::TEventType;
  using TPersonaGenerateFn = std::size_t (*)(void*,
                                             const TChordEventSet&,
                                             EVENT_TYPE&);
  using TPersonaChordsFn = std::size_t (*)(void*, EVENT_TYPE&);

  template<typename TRole>
  using TPersona = Persona<TRole, MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;

public:
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  EventSetManager(const TSetup& setup)
    : setup_(&setup)

  {
    events_.clear();
    event_text_.clear();
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param persona
  template<typename TRole>
  void set_persona(TPersona<TRole>& persona)
  {
    persona_context_ = &persona;

    if constexpr (std::is_same_v<EVENT_TYPE, TNoteEventSet>) {
      persona_generate_ = [](void* context,
                             const TChordEventSet& chord_events,
                             EVENT_TYPE& events) -> std::size_t {
        auto* persona = static_cast<TPersona<TRole>*>(context);
        return persona->GenerateNoteEvents(chord_events, events);
      };
    } else {
      persona_generate_ = nullptr;
    }

    if constexpr (std::is_same_v<EVENT_TYPE, TChordEventSet>) {
      persona_chords_ = [](void* context, EVENT_TYPE& events) -> std::size_t {
        auto* persona = static_cast<TPersona<TRole>*>(context);
        return persona->generate_chord_events(events);
      };
    } else {
      persona_chords_ = nullptr;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  void clear_persona()
  {
    persona_context_ = nullptr;
    persona_generate_ = nullptr;
    persona_chords_ = nullptr;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param chord_events
  /// @return
  std::size_t make_note_events_from_chords(const TChordEventSet& chord_events)
  {
    events_.clear();
    current_index_ = 0;

    if (persona_generate_ == nullptr) {
      return events_.size();
    }

    return persona_generate_(persona_context_, chord_events, events_);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param granularity
  /// @return
  std::size_t make_chord_events()
  {
    events_.clear();
    current_index_ = 0;

    if (persona_chords_ == nullptr) {
      return events_.size();
    }
    return persona_chords_(persona_context_, events_);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const TNoteEvent& get_current_note() const
  {
    return events_[current_index_];
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const TChordEvent& get_current_chord() const
  {
    return events_[current_index_];
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const char* get_text() const { return event_text_.c_str(); }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  const EVENT_TYPE& get_events() { return events_; }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] bool get_gate() const { return gate_; }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] bool get_trigger() const { return trigger_; }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param pulse
  void handle_pulse(int pulse)
  {
    current_index_ = events_.get_event_index_for_pulse(pulse);
    if (current_index_ < 0 ||
        current_index_ >= static_cast<int>(events_.size())) {
      gate_ = false;
      trigger_ = false;
      return;
    }

    if (is_event_rising_edge(pulse)) {
      gate_ = events_.is_hit(static_cast<std::size_t>(current_index_));
      trigger_ = gate_;

      update_event_text();

    } else {
      // Trigger is only on the rising edge of the of an event
      trigger_ = false;

      if (is_event_falling_edge(pulse)) {
        gate_ = false;
        event_text_.clear();
      }
    }
  }

protected:
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param pulse
  /// @return
  [[nodiscard]] bool is_event_rising_edge(int pulse) const
  {
    return get_event_pulse_offset(pulse) == 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param pulse
  /// @param articulation
  /// @return
  [[nodiscard]] bool is_event_falling_edge(
    int pulse,
    Articulation articulation = Articulation::Normal) const
  {
    if (events_.size() == 0 || pulse < 0) {
      return false;
    }

    const int total_pulses = events_.get_total_event_pulses();
    if (total_pulses <= 0) {
      return false;
    }

    int previous_pulse = pulse - 1;
    if (pulse == 0) {
      if (total_pulses > 0) {
        previous_pulse = total_pulses - 1;
      }
    }

    const TEvent& current_event = events_.get_event_for_pulse(pulse);
    const TEvent& previous_event = events_.get_event_for_pulse(previous_pulse);

    // Always release when the sequence transitions from note to rest.
    if (EventTraits<TEvent>::is_hit(previous_event) &&
        !EventTraits<TEvent>::is_hit(current_event)) {
      return true;
    }

    // Legato keeps the gate high between adjacent note events_.
    if (articulation == Articulation::Legato ||
        !EventTraits<TEvent>::is_hit(current_event)) {
      return false;
    }

    const int event_idx = events_.get_event_index_for_pulse(pulse);
    if (event_idx < 0 || event_idx >= static_cast<int>(events_.size())) {
      return false;
    }

    const int event_pulse_offset = get_event_pulse_offset(pulse);
    if (event_pulse_offset < 0) {
      return false;
    }

    const int span =
      static_cast<int>(events_.value(static_cast<std::size_t>(event_idx)));
    if (span <= 1) {
      return true;
    }

    float gate_fraction = 0.90F; // Normal articulation.
    if (articulation == Articulation::Staccato) {
      gate_fraction = 0.55F;
    }

    int release_pulse_offset = static_cast<int>(span * gate_fraction);
    release_pulse_offset = std::max(release_pulse_offset, 0);
    release_pulse_offset = std::min(release_pulse_offset, span - 1);

    return event_pulse_offset == release_pulse_offset;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param pulse
  /// @return
  [[nodiscard]] int get_event_pulse_offset(int pulse) const
  {
    if (events_.size() == 0 || pulse < 0) {
      return -1;
    }

    const int total_pulses = events_.get_total_event_pulses();
    if (total_pulses <= 0) {
      return -1;
    }

    const int event_idx = events_.get_event_index_for_pulse(pulse);
    if (event_idx < 0 || event_idx >= static_cast<int>(events_.size())) {
      return -1;
    }

    const int event_start_pulse =
      events_.get_event_start_pulse(static_cast<std::size_t>(event_idx));
    return ((pulse % total_pulses) - event_start_pulse + total_pulses) %
           total_pulses;
  }

private:
  const TSetup* setup_;
  EVENT_TYPE events_;
  int current_index_{ 0 };
  void* persona_context_{ nullptr };
  TPersonaGenerateFn persona_generate_{ nullptr };
  TPersonaChordsFn persona_chords_{ nullptr };
  MString<16> event_text_;
  bool gate_{ false };
  bool trigger_{ false };

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  void update_event_text()
  {
    if constexpr (std::is_same_v<EVENT_TYPE, TChordEventSet>) {
      //      get_current_chord().get_chord_name(events_.
      get_current_chord().get_chord_name(
        setup_->scale_map, setup_->temperament, event_text_);
    } else if constexpr (std::is_same_v<EVENT_TYPE, TNoteEventSet>) {
      auto& n = get_current_note();
      if (n.IsPitched()) {
        n.get_interval_name(setup_->temperament, event_text_);
      } else {
        event_text_.clear();
      }
    } else {
      event_text_.clear();
    }
  }
};
} // namespace Music
