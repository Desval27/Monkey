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
    eventText_.clear();
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param persona
  template<typename TRole>
  void set_persona(TPersona<TRole>& persona)
  {
    personaContext_ = &persona;

    if constexpr (std::is_same_v<EVENT_TYPE, TNoteEventSet>) {
      personaGenerate_ = [](void* context,
                            const TChordEventSet& chordEvents,
                            EVENT_TYPE& events) -> std::size_t {
        auto* persona = static_cast<TPersona<TRole>*>(context);
        return persona->GenerateNoteEvents(chordEvents, events);
      };
    } else {
      personaGenerate_ = nullptr;
    }

    if constexpr (std::is_same_v<EVENT_TYPE, TChordEventSet>) {
      personaChords_ = [](void* context, EVENT_TYPE& events) -> std::size_t {
        auto* persona = static_cast<TPersona<TRole>*>(context);
        return persona->GenerateChordEvents(events);
      };
    } else {
      personaChords_ = nullptr;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  void clear_persona()
  {
    personaContext_ = nullptr;
    personaGenerate_ = nullptr;
    personaChords_ = nullptr;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param chordEvents
  /// @return
  std::size_t make_note_events_from_chords(const TChordEventSet& chordEvents)
  {
    events_.clear();
    currentIndex_ = 0;

    if (personaGenerate_ == nullptr) {
      return events_.size();
    }

    return personaGenerate_(personaContext_, chordEvents, events_);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param granularity
  /// @return
  std::size_t make_chord_events()
  {
    events_.clear();
    currentIndex_ = 0;

    if (personaChords_ == nullptr) {
      return events_.size();
    }
    return personaChords_(personaContext_, events_);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const TNoteEvent& get_current_note() const
  {
    return events_[currentIndex_];
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const TChordEvent& get_current_chord() const
  {
    return events_[currentIndex_];
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const char* get_text() const { return eventText_.c_str(); }

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
    currentIndex_ = events_.get_event_index_for_pulse(pulse);
    if (currentIndex_ < 0 ||
        currentIndex_ >= static_cast<int>(events_.size())) {
      gate_ = false;
      trigger_ = false;
      return;
    }

    if (is_event_rising_edge(pulse)) {
      gate_ = events_.is_hit(static_cast<std::size_t>(currentIndex_));
      trigger_ = gate_;

      update_event_text();

    } else {
      // Trigger is only on the rising edge of the of an event
      trigger_ = false;

      if (is_event_falling_edge(pulse)) {
        gate_ = false;
        eventText_.clear();
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

    const int totalPulses = events_.get_total_event_pulses();
    if (totalPulses <= 0) {
      return false;
    }

    int previousPulse = pulse - 1;
    if (pulse == 0) {
      if (totalPulses > 0) {
        previousPulse = totalPulses - 1;
      }
    }

    const TEvent& currentEvent = events_.get_event_for_pulse(pulse);
    const TEvent& previousEvent = events_.get_event_for_pulse(previousPulse);

    // Always release when the sequence transitions from note to rest.
    if (EventTraits<TEvent>::is_hit(previousEvent) &&
        !EventTraits<TEvent>::is_hit(currentEvent)) {
      return true;
    }

    // Legato keeps the gate high between adjacent note events_.
    if (articulation == Articulation::Legato ||
        !EventTraits<TEvent>::is_hit(currentEvent)) {
      return false;
    }

    const int eventIdx = events_.get_event_index_for_pulse(pulse);
    if (eventIdx < 0 || eventIdx >= static_cast<int>(events_.size())) {
      return false;
    }

    const int eventPulseOffset = get_event_pulse_offset(pulse);
    if (eventPulseOffset < 0) {
      return false;
    }

    const int span =
      static_cast<int>(events_.value(static_cast<std::size_t>(eventIdx)));
    if (span <= 1) {
      return true;
    }

    float gateFraction = 0.90F; // Normal articulation.
    if (articulation == Articulation::Staccato) {
      gateFraction = 0.55F;
    }

    int releasePulseOffset = static_cast<int>(span * gateFraction);
    releasePulseOffset = std::max(releasePulseOffset, 0);
    releasePulseOffset = std::min(releasePulseOffset, span - 1);

    return eventPulseOffset == releasePulseOffset;
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

    const int totalPulses = events_.get_total_event_pulses();
    if (totalPulses <= 0) {
      return -1;
    }

    const int eventIdx = events_.get_event_index_for_pulse(pulse);
    if (eventIdx < 0 || eventIdx >= static_cast<int>(events_.size())) {
      return -1;
    }

    const int eventStartPulse =
      events_.get_event_start_pulse(static_cast<std::size_t>(eventIdx));
    return ((pulse % totalPulses) - eventStartPulse + totalPulses) %
           totalPulses;
  }

private:
  const TSetup* setup_;
  EVENT_TYPE events_;
  int currentIndex_{ 0 };
  void* personaContext_{ nullptr };
  TPersonaGenerateFn personaGenerate_{ nullptr };
  TPersonaChordsFn personaChords_{ nullptr };
  MString<16> eventText_;
  bool gate_{ false };
  bool trigger_{ false };

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  void update_event_text()
  {
    if constexpr (std::is_same_v<EVENT_TYPE, TChordEventSet>) {
      //      get_current_chord().get_chord_name(events_.
      get_current_chord().get_chord_name(
        setup_->scale_map, setup_->temperament, eventText_);
    } else if constexpr (std::is_same_v<EVENT_TYPE, TNoteEventSet>) {
      auto& n = get_current_note();
      if (n.IsPitched()) {
        n.get_interval_name(setup_->temperament, eventText_);
      } else {
        eventText_.clear();
      }
    } else {
      eventText_.clear();
    }
  }
};
} // namespace Music
