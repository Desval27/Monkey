#pragma once

#include <cstddef>
#include <utility>

#include <Music/MusicConfig.h>
#include <Music/TimeSignature.h>
#include <Music/Chord.h>
#include <Music/Note.h>

namespace Music
{
  template <typename T>
  struct EventTraits;

  template <>
  struct EventTraits<NoteEvent>
  {
    static Note &Pitch(NoteEvent &event) { return event.note; }
    static const Note &Pitch(const NoteEvent &event) { return event.note; }
    static NoteValue &Value(NoteEvent &event) { return event.value; }
    static const NoteValue &Value(const NoteEvent &event) { return event.value; }
    static bool IsHit(const NoteEvent &event) { return event.note != REST; }
  };

  template <std::size_t DEGREES>
  struct EventTraits<ChordEvent<DEGREES>>
  {
    static Note &Pitch(ChordEvent<DEGREES> &event) { return event.root; }
    static const Note &Pitch(const ChordEvent<DEGREES> &event) { return event.root; }
    static NoteValue &Value(ChordEvent<DEGREES> &event) { return event.value; }
    static const NoteValue &Value(const ChordEvent<DEGREES> &event) { return event.value; }
    static bool IsHit(const ChordEvent<DEGREES> &event) { return event.root != REST; }
  };

  template <>
  struct EventTraits<bool>
  {
    static bool IsHit(const bool &event) { return event; }
  };

  template <typename T, size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  class EventSet
  {
  public:
    using EventType = T;

    EventSet() : emptyEvent_(), eventCount_(0) {}

    void Clear() { eventCount_ = 0; }

    bool AtCapacity() const { return Count() >= Capacity() - 1; }
    constexpr size_t Capacity() const { return MAX_EVENTS; }
    size_t Count() const { return eventCount_; }
    bool IsEmpty() const { return eventCount_ == 0; }

    int GetBarsForTimeSignature(const TimeSignature &ts) const
    {
      int ppb = ts.GetPulsesPerBar();
      if (ppb != 0)
        return GetTotalEventPulses() / ppb;
      return 0;
    }

    int GetEventStartPulse(size_t index) const
    {
      int pulseCursor = 0;
      const size_t end = index < Count() ? index : Count();
      for (size_t i = 0; i < end; i++)
        pulseCursor += static_cast<int>(Value(i));
      return pulseCursor;
    }

    int GetTotalEventPulses() const
    {
      int totalPulses = 0;
      for (size_t i = 0; i < Count(); i++)
        totalPulses += static_cast<int>(Value(i));
      return totalPulses;
    }

    int GetEventIndexForPulse(int pulse) const
    {
      if (Count() == 0 || pulse < 0)
        return -1;

      int totalPulses = GetTotalEventPulses();

      if (totalPulses <= 0)
        return -1;

      const int normalizedPulse = pulse % totalPulses;

      int pulseCursor = 0;
      for (size_t i = 0; i < Count(); i++)
      {
        const int span = static_cast<int>(Value(i));
        if (normalizedPulse < (pulseCursor + span))
          return static_cast<int>(i);

        pulseCursor += span;
      }

      return static_cast<int>(Count() - 1);
    }

    const T &GetEventForPulse(int pulse) const
    {
      int index = GetEventIndexForPulse(pulse);
      if (index >= 0 && index < static_cast<int>(Count()))
        return events_[index];
      return emptyEvent_;
    }

    T *Data() { return events_; }
    const T *Data() const { return events_; }

    T &operator[](size_t index) { return events_[index]; }
    const T &operator[](size_t index) const { return events_[index]; }

    bool Add(const T &event)
    {
      if (eventCount_ >= MAX_EVENTS)
      {
        return false;
      }

      events_[eventCount_++] = event;
      return true;
    }

    template <typename... Args>
    bool Emplace(Args &&...args)
    {
      if (eventCount_ >= MAX_EVENTS)
      {
        return false;
      }

      events_[eventCount_++] = T(std::forward<Args>(args)...);
      return true;
    }

    float GetDensity() const 
    {
      int hitCount = 0;
      for (size_t i = 0; i < Count(); i++)
        if (IsHit(i))
          hitCount++;
      return (float)hitCount/(float)Count();
    }

    Note &Pitch(size_t index) { return EventTraits<T>::Pitch(events_[index]); }
    const Note &Pitch(size_t index) const
    {
      return EventTraits<T>::Pitch(events_[index]);
    }

    NoteValue &Value(size_t index) { return EventTraits<T>::Value(events_[index]); }
    const NoteValue &Value(size_t index) const
    {
      return EventTraits<T>::Value(events_[index]);
    }

    bool IsHit(size_t index) const { return EventTraits<T>::IsHit(events_[index]); }

  private:
    T emptyEvent_;
    T events_[MAX_EVENTS];
    size_t eventCount_;
  };

  template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  using PatternEventSet = EventSet<bool, MAX_EVENTS>;

  template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  using NoteEventSet = EventSet<NoteEvent, MAX_EVENTS>;

  template <size_t DEGREES = HEPATONIC, size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  using ChordEventSet = EventSet<ChordEvent<DEGREES>, MAX_EVENTS>;

} // namespace Music
