#pragma once

#include <algorithm>

#include <music/event_set.hpp>
#include <music/persona.hpp>

namespace music {

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS,
         typename EVENT_TYPE = NoteEventSet<MAX_EVENTS>>
class EventSetManager
{
  using MyNoteEvent = NoteEvent;
  using MyChordEvent = ChordEvent<MAX_DEGREES, SCALE_DEGREES>;
  using MyChordEventSet = ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
  using PersonaGenerateFn = std::size_t (*)(void*,
                                            const MyChordEventSet&,
                                            EVENT_TYPE&);

  template<typename TRole>
  using MyPersona = Persona<TRole, MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;

public:
  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  EventSetManager()
  {
    events_.clear();
    eventText_.clear();
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param persona
  template<typename TRole>
  void set_persona(MyPersona<TRole>& persona)
  {
    personaContext_ = &persona;
    personaGenerate_ = [](void* context,
                          const MyChordEventSet& chordEvents,
                          EVENT_TYPE& events) -> std::size_t {
      auto* persona = static_cast<MyPersona<TRole>*>(context);
      return persona->GenerateNoteEvents(chordEvents, events);
    };
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  void clear_persona()
  {
    personaContext_ = nullptr;
    personaGenerate_ = nullptr;
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param chordEvents
  /// @return
  std::size_t make_note_events_from_chords(const MyChordEventSet& chordEvents)
  {
    events_.clear();
    currentIndex_ = 0;

    if (personaGenerate_ == nullptr) {
      return events_.size();
    }

    return personaGenerate_(personaContext_, chordEvents, events_);
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const MyNoteEvent& get_current_note() const
  {
    return events_[currentIndex_];
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const MyChordEvent& get_current_chord() const
  {
    return events_[currentIndex_];
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const char* get_text() const { return eventText_.c_str(); }

  const EVENT_TYPE& get_events() { return events_; }

  bool get_gate() const { return gate_; }
  bool get_trigger() const { return trigger_; }

  ///////////////////////////////////////////////////////////////////////////
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

    const NoteEvent& event = events_[currentIndex_];
    if (is_event_rising_edge(pulse)) {
      gate_ = event.IsPitched();
      trigger_ = gate_;
    } else {
      // Trigger is only on the rising edge of the of an event
      trigger_ = false;

      if (is_event_falling_edge(pulse)) {
        gate_ = false;
      }
    }
  }

protected:
  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param pulse
  /// @return
  [[nodiscard]] bool is_event_rising_edge(int pulse) const
  {
    return get_event_pulse_offset(pulse) == 0;
  }

  ///////////////////////////////////////////////////////////////////////////
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

    const NoteEvent& currentEvent = events_.get_event_for_pulse(pulse);
    const NoteEvent& previousEvent = events_.get_event_for_pulse(previousPulse);

    // Always release when the sequence transitions from note to rest.
    if (previousEvent.note != REST && currentEvent.note == REST) {
      return true;
    }

    // Legato keeps the gate high between adjacent note events_.
    if (articulation == Articulation::Legato || currentEvent.note == REST) {
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

    const int span = static_cast<int>(events_[eventIdx].value);
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

  ///////////////////////////////////////////////////////////////////////////
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
  EVENT_TYPE events_;
  int currentIndex_{ 0 };
  void* personaContext_{ nullptr };
  PersonaGenerateFn personaGenerate_;
  MString<16> eventText_;
  bool gate_{ false };
  bool trigger_{ false };
};
} // namespace Music
