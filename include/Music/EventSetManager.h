#pragma once

#include <Music/EventSet.h>
#include <Music/NoteGenerator.h>
#include <Music/Persona.h>
#include <Music/ScaleMap.h>
#include <Music/Setup.h>
#include <Music/Temperament.h>

#include <algorithm>

namespace Music {

template <std::size_t MAX_DEGREES = Music::DEF_MAX_DEGREES,
          std::size_t SCALE_DEGREES = Music::DEF_SCALE_DEGREES,
          std::size_t MAX_EVENTS = Music::DEF_MAX_EVENTS,
          typename EVENT_TYPE = Music::NoteEventSet<MAX_EVENTS>>
class EventSetManager {
  using MyNoteEvent = Music::NoteEvent;
  using MyChordEvent = Music::ChordEvent<MAX_DEGREES, SCALE_DEGREES>;
  using MyChordEventSet =
      Music::ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
  using PersonaGenerateFn = std::size_t (*)(void *, const MyChordEventSet &,
                                            EVENT_TYPE &);

  template <typename TRole>
  using MyPersona =
      Music::Persona<TRole, MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;

public:
  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  EventSetManager() {
    events_.Clear();
    eventText_.clear();
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param persona
  template <typename TRole> void SetPersona(MyPersona<TRole> &persona) {
    personaContext_ = &persona;
    personaGenerate_ = [](void *context, const MyChordEventSet &chordEvents,
                          EVENT_TYPE &events) -> std::size_t {
      auto *persona = static_cast<MyPersona<TRole> *>(context);
      return persona->GenerateNoteEvents(chordEvents, events);
    };
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  void ClearPersona() {
    personaContext_ = nullptr;
    personaGenerate_ = nullptr;
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param chordEvents
  /// @return
  std::size_t MakeNoteEventsFromChords(const MyChordEventSet &chordEvents) {
    events_.Clear();
    currentEvent_ = 0;

    if (personaGenerate_ == nullptr) {
      return events_.Count();
    }

    return personaGenerate_(personaContext_, chordEvents, events_);
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const MyNoteEvent &GetCurrentNote() const {
    return events_[currentEvent_];
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const MyChordEvent &GetCurrentChord() const {
    return events_[currentEvent_];
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const char *GetText() const { return eventText_.c_str(); }

  const EVENT_TYPE &GetEvents() { return events_; }

protected:
  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param pulse
  /// @return
  [[nodiscard]] bool IsEventRisingEdge(int pulse) const {
    return GetEventPulseOffset(pulse) == 0;
  }

  ///////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param pulse
  /// @param articulation
  /// @return
  [[nodiscard]] bool IsEventFallingEdge(
      int pulse,
      Music::Articulation articulation = Music::Articulation::Normal) const {
    if (events_.Count() == 0 || pulse < 0) {
      return false;
    }

    const int totalPulses = events_.GetTotalEventPulses();
    if (totalPulses <= 0) {
      return false;
    }

    int previousPulse = pulse - 1;
    if (pulse == 0) {
      if (totalPulses > 0) {
        previousPulse = totalPulses - 1;
      }
    }

    const Music::NoteEvent &currentEvent = events_.GetEventForPulse(pulse);
    const Music::NoteEvent &previousEvent =
        events_.GetEventForPulse(previousPulse);

    // Always release when the sequence transitions from note to rest.
    if (previousEvent.note != Music::REST && currentEvent.note == Music::REST) {
      return true;
    }

    // Legato keeps the gate high between adjacent note events_.
    if (articulation == Music::Articulation::Legato ||
        currentEvent.note == Music::REST) {
      return false;
    }

    const int eventIdx = events_.GetEventIndexForPulse(pulse);
    if (eventIdx < 0 || eventIdx >= static_cast<int>(events_.Count())) {
      return false;
    }

    const int eventPulseOffset = GetEventPulseOffset(pulse);
    if (eventPulseOffset < 0) {
      return false;
    }

    const int span = static_cast<int>(events_[eventIdx].value);
    if (span <= 1) {
      return true;
    }

    float gateFraction = 0.90F; // Normal articulation.
    if (articulation == Music::Articulation::Staccato) {
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
  [[nodiscard]] int GetEventPulseOffset(int pulse) const {
    if (events_.Count() == 0 || pulse < 0) {
      return -1;
    }

    const int totalPulses = events_.GetTotalEventPulses();
    if (totalPulses <= 0) {
      return -1;
    }

    const int eventIdx = events_.GetEventIndexForPulse(pulse);
    if (eventIdx < 0 || eventIdx >= static_cast<int>(events_.Count())) {
      return -1;
    }

    const int eventStartPulse =
        events_.GetEventStartPulse(static_cast<std::size_t>(eventIdx));
    return ((pulse % totalPulses) - eventStartPulse + totalPulses) %
           totalPulses;
  }

private:
  EVENT_TYPE events_;
  int currentEvent_{0};
  void *personaContext_{nullptr};
  PersonaGenerateFn personaGenerate_;
  MString<16> eventText_;
};
} // namespace Music
