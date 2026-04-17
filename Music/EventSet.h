#pragma once

#include <cstddef>

#include "Note.h"
#include "Chord.h"

namespace Music {

template <typename T>
struct EventTraits;

template <>
struct EventTraits<NoteEvent> {
  static Note& Pitch(NoteEvent& event) { return event.note; }
  static const Note& Pitch(const NoteEvent& event) { return event.note; }
  static NoteValue& Value(NoteEvent& event) { return event.value; }
  static const NoteValue& Value(const NoteEvent& event) { return event.value; }
};

template <>
struct EventTraits<ChordEvent> {
  static Note& Pitch(ChordEvent& event) { return event.root; }
  static const Note& Pitch(const ChordEvent& event) { return event.root; }
  static NoteValue& Value(ChordEvent& event) { return event.value; }
  static const NoteValue& Value(const ChordEvent& event) { return event.value; }
};

template <typename T, size_t MAX_EVENTS = 128>
class EventSet {
 public:
  using EventType = T;

  EventSet() : _eventCount(0) {}

  constexpr size_t Capacity() const { return MAX_EVENTS; }
  size_t Size() const { return _eventCount; }
  bool Empty() const { return _eventCount == 0; }
  void Clear() { _eventCount = 0; }

  T* Data() { return _events; }
  const T* Data() const { return _events; }

  T& operator[](size_t index) { return _events[index]; }
  const T& operator[](size_t index) const { return _events[index]; }

  bool Add(const T& event) {
    if (_eventCount >= MAX_EVENTS) {
      return false;
    }

    _events[_eventCount++] = event;
    return true;
  }

  template <typename... Args>
  bool Emplace(Args&&... args) {
    if (_eventCount >= MAX_EVENTS) {
      return false;
    }

    _events[_eventCount++] = T{static_cast<Args&&>(args)...};
    return true;
  }

  Note& Pitch(size_t index) { return EventTraits<T>::Pitch(_events[index]); }
  const Note& Pitch(size_t index) const {
    return EventTraits<T>::Pitch(_events[index]);
  }

  NoteValue& Value(size_t index) { return EventTraits<T>::Value(_events[index]); }
  const NoteValue& Value(size_t index) const {
    return EventTraits<T>::Value(_events[index]);
  }

 private:
  T _events[MAX_EVENTS];
  size_t _eventCount;
};

template <size_t MAX_EVENTS = 128>
using NoteEventSet = EventSet<NoteEvent, MAX_EVENTS>;

template <size_t MAX_EVENTS = 128>
using ChordEventSet = EventSet<ChordEvent, MAX_EVENTS>;

}  // namespace Music
