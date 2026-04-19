#pragma once

#include <cstddef>
#include <utility>

#include <Music/Chord.h>
#include <Music/Note.h>

namespace Music
{

  constexpr size_t DEFAULT_MAX_EVENTS = 128;

  template <typename T>
  struct EventTraits;

  template <>
  struct EventTraits<NoteEvent>
  {
    static Note &Pitch(NoteEvent &event) { return event.note; }
    static const Note &Pitch(const NoteEvent &event) { return event.note; }
    static NoteValue &Value(NoteEvent &event) { return event.value; }
    static const NoteValue &Value(const NoteEvent &event) { return event.value; }
  };

  template <>
  struct EventTraits<ChordEvent>
  {
    static Note &Pitch(ChordEvent &event) { return event.root; }
    static const Note &Pitch(const ChordEvent &event) { return event.root; }
    static NoteValue &Value(ChordEvent &event) { return event.value; }
    static const NoteValue &Value(const ChordEvent &event) { return event.value; }
  };

  template <typename T, size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  class EventSet
  {
  public:
    using EventType = T;

    EventSet() : _emptyEvent(), _eventCount(0) {}

    constexpr size_t Capacity() const { return MAX_EVENTS; }
    size_t Count() const { return _eventCount; }
    bool Empty() const { return _eventCount == 0; }
    void Clear() { _eventCount = 0; }

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
        return _events[index];
      return _emptyEvent;
    }

    T *Data() { return _events; }
    const T *Data() const { return _events; }

    T &operator[](size_t index) { return _events[index]; }
    const T &operator[](size_t index) const { return _events[index]; }

    bool Add(const T &event)
    {
      if (_eventCount >= MAX_EVENTS)
      {
        return false;
      }

      _events[_eventCount++] = event;
      return true;
    }

    template <typename... Args>
    bool Emplace(Args &&...args)
    {
      if (_eventCount >= MAX_EVENTS)
      {
        return false;
      }

      _events[_eventCount++] = T(std::forward<Args>(args)...);
      return true;
    }

    Note &Pitch(size_t index) { return EventTraits<T>::Pitch(_events[index]); }
    const Note &Pitch(size_t index) const
    {
      return EventTraits<T>::Pitch(_events[index]);
    }

    NoteValue &Value(size_t index) { return EventTraits<T>::Value(_events[index]); }
    const NoteValue &Value(size_t index) const
    {
      return EventTraits<T>::Value(_events[index]);
    }

  private:
    T _emptyEvent;
    T _events[MAX_EVENTS];
    size_t _eventCount;
  };

  template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  using PatternEventSet = EventSet<bool, MAX_EVENTS>;

  template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  using NoteEventSet = EventSet<NoteEvent, MAX_EVENTS>;

  template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  using ChordEventSet = EventSet<ChordEvent, MAX_EVENTS>;

} // namespace Music
