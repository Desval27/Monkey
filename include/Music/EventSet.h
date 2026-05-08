#pragma once

#include <cstddef>
#include <utility>

#include <Music/Chord.h>
#include <Music/MusicConfig.h>
#include <Music/Note.h>
#include <Music/TimeSignature.h>

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

  template <std::size_t MAX_DEGREES, std::size_t SCALE_DEGREES>
  struct EventTraits<ChordEvent<MAX_DEGREES, SCALE_DEGREES>>
  {
    static Note &Pitch(ChordEvent<MAX_DEGREES, SCALE_DEGREES> &event)
    {
      return event.root;
    }
    static const Note &
    Pitch(const ChordEvent<MAX_DEGREES, SCALE_DEGREES> &event)
    {
      return event.root;
    }
    static NoteValue &Value(ChordEvent<MAX_DEGREES, SCALE_DEGREES> &event)
    {
      return event.value;
    }
    static const NoteValue &
    Value(const ChordEvent<MAX_DEGREES, SCALE_DEGREES> &event)
    {
      return event.value;
    }
    static bool IsHit(const ChordEvent<MAX_DEGREES, SCALE_DEGREES> &event)
    {
      return event.root != REST;
    }
  };

  template <>
  struct EventTraits<bool>
  {
    static bool IsHit(const bool &event) { return event; }
  };

  /**
   * @brief A generic container for musical events, such as notes or chords, that
   * occur at specific time intervals. This class provides functionality to manage
   * a sequence of events, including adding new events, calculating event density,
   * and determining event positions based on time signatures. It is designed to
   * be flexible and can be specialized for different types of musical events by
   * defining appropriate traits. The EventSet class is intended to be used as a
   * base for specific event types, such as NoteEventSet and ChordEventSet, which
   * can be defined using type aliases with specific event types and maximum
   * capacities.
   */
  template <typename T, std::size_t MAX_EVENTS>
  class EventSet
  {
  public:
    using EventType = T;

    EventSet() = default;

    /**
     * @brief Clears all events from the set, resetting the event count to zero.
     * This effectively removes all stored events and allows for new events to be
     * added from an empty state.
     */
    void Clear() { eventCount_ = 0; }

    /**
     * @brief Checks if the event set has reached its maximum capacity. This
     * function returns true if the number of events currently stored in the set
     * is greater than or equal to the defined maximum capacity, indicating that
     * no additional events can be added until some are removed or cleared.
     * @return bool True if the event set is at capacity, false otherwise.
     */
    [[nodiscard]] bool AtCapacity() const { return Count() >= Capacity() - 1; }

    /**
     * @brief Returns the maximum number of events that the set can hold, as
     * defined by the template parameter MAX_EVENTS. This function provides a way
     * to query the capacity of the event set, which is useful for managing event
     * additions and ensuring that the set does not exceed its limits.
     * @return std::size_t The maximum number of events the set can hold.
     */
    [[nodiscard]] constexpr std::size_t Capacity() const { return MAX_EVENTS; }

    /**
     * @brief Returns the current number of events stored in the set. This
     * function allows users to determine how many events have been added to the
     * set, which is important for managing event sequences and ensuring that
     * operations such as adding new events or clearing the set are performed
     * correctly.
     * @return std::size_t The number of events currently stored in the set.
     */
    [[nodiscard]] std::size_t Count() const { return eventCount_; }

    /**
     * @brief Checks if the event set is empty, meaning that it contains no
     * events. This function returns true if the event count is zero, indicating
     * that there are no events currently stored in the set, and false otherwise.
     * An empty event set can be useful for initializing new sequences or
     * resetting the state of the set before adding new events.
     * @return bool True if the event set is empty, false otherwise.
     */
    [[nodiscard]] bool IsEmpty() const { return eventCount_ == 0; }

    /**
     * @brief Calculates the number of bars that can be formed by the events in
     * the set based on a given time signature. This function uses the total
     * number of pulses represented by the events and divides it by the number of
     * pulses per bar as defined by the time signature. It returns the total
     * number of complete bars that can be formed with the current events, which
     * is useful for understanding the structure and length of the musical
     * sequence represented by the event set.
     * @param ts The time signature used to calculate the number of bars.
     * @return int The number of bars that can be formed by the events in the set
     * based on the provided time signature.
     */
    [[nodiscard]] int GetBarsForTimeSignature(const TimeSignature &ts) const
    {
      int ppb = ts.GetPulsesPerBar();
      if (ppb != 0)
      {
        return GetTotalEventPulses() / ppb;
      }
      return 0;
    }

    /**
     * @brief Calculates the starting pulse position of a specific event in the
     * set based on its index. This function iterates through the events up to the
     * specified index, summing their durations (in pulses) to determine the
     * cumulative pulse count at which the event starts. If the index is out of
     * bounds (greater than or equal to the number of events), it calculates the
     * starting pulse position as if the index were at the end of the event list,
     * effectively giving the total duration of all events in pulses.
     * @param index The index of the event for which to calculate the starting
     * pulse position. If the index is out of bounds, it will be treated as if it
     * were at the end of the event list.
     * @return int The starting pulse position of the specified event in the set.
     */
    [[nodiscard]] int GetEventStartPulse(std::size_t index) const
    {
      int pulseCursor = 0;
      const std::size_t end = index < Count() ? index : Count();
      for (std::size_t i = 0; i < end; i++)
      {
        pulseCursor += static_cast<int>(Value(i));
      }
      return pulseCursor;
    }

    /**
     * @brief Calculates the total number of pulses represented by all events in
     * the set. This function iterates through all events in the set, summing
     * their durations (in pulses) to determine the total pulse count. This total
     * can be used to understand the overall length of the musical sequence
     * represented by the event set and is essential for operations such as
     * calculating bars based on time signatures or determining event positions in
     * a timeline.
     * @return int The total number of pulses represented by all events in the
     * set.
     */
    [[nodiscard]] int GetTotalEventPulses() const
    {
      int totalPulses = 0;
      for (std::size_t i = 0; i < Count(); i++)
      {
        totalPulses += static_cast<int>(Value(i));
      }
      return totalPulses;
    }

    /**
     * @brief Determines the index of the event that corresponds to a specific
     * pulse position in the timeline. This function first checks if the event set
     * is empty or if the pulse value is negative, in which case it returns -1 to
     * indicate an invalid index. It then normalizes the pulse value by taking it
     * modulo the total number of pulses represented by all events, allowing for
     * cyclic behavior. The function iterates through the events, keeping a
     * running total of pulses, and returns the index of the event where the
     * normalized pulse falls within its duration. If the pulse exceeds the total
     * duration of all events, it returns the index of the last event.
     * @param pulse The pulse position for which to determine the corresponding
     * event index. This value can be any non-negative integer, and it will be
     * normalized to fit within the total duration of the events in the set.
     * @return int The index of the event that corresponds to the specified pulse
     * position, or -1 if the event set is empty or the pulse value is invalid.
     */
    [[nodiscard]] int GetEventIndexForPulse(int pulse) const
    {
      if (Count() == 0 || pulse < 0)
      {
        return -1;
      }

      int totalPulses = GetTotalEventPulses();

      if (totalPulses <= 0)
      {
        return -1;
      }

      const int normalizedPulse = pulse % totalPulses;

      int pulseCursor = 0;
      for (std::size_t i = 0; i < Count(); i++)
      {
        const int span = static_cast<int>(Value(i));
        if (normalizedPulse < (pulseCursor + span))
        {
          return static_cast<int>(i);
        }

        pulseCursor += span;
      }

      return static_cast<int>(Count() - 1);
    }

    /**
     * @brief Retrieves the event that corresponds to a specific pulse position in
     * the timeline. This function uses the GetEventIndexForPulse method to
     * determine the index of the event at the given pulse position and returns a
     * reference to that event. If the index is valid (within the bounds of the
     * event count), it returns the corresponding event; otherwise, it returns a
     * reference to an empty event, which can be used as a default value when no
     * valid event is found for the specified pulse.
     * @param pulse The pulse position for which to retrieve the corresponding
     * event. This value can be any non-negative integer, and it will be
     * normalized to fit within the total duration of the events in the set.
     * @return const T& A reference to the event that corresponds to the specified
     * pulse position, or a reference to an empty event if the index is invalid.
     */
    [[nodiscard]] const T &GetEventForPulse(int pulse) const
    {
      int index = GetEventIndexForPulse(pulse);
      if (index >= 0 && index < static_cast<int>(Count()))
      {
        return events_[index];
      }
      return emptyEvent_;
    }

    /**
     * @brief Provides direct access to the underlying array of events in the set.
     * This function returns a pointer to the first event in the array, allowing
     * for iteration or manipulation of the events directly. It is important to
     * note that the caller should ensure that they do not access beyond the
     * current count of events, as this could lead to undefined behavior. The
     * const version of this function allows for read-only access to the events,
     * ensuring that the caller cannot modify the event data when using a const
     * reference to the EventSet.
     * @return T* A pointer to the first event in the array, allowing for direct
     * access to the events in the set. The caller should ensure that they do not
     * access beyond the current count of events.
     */
    T *Data() { return events_; }

    /**
     * @brief Provides direct access to the underlying array of events in the set.
     * This function returns a pointer to the first event in the array, allowing
     * for iteration or manipulation of the events directly. It is important to
     * note that the caller should ensure that they do not access beyond the
     * current count of events, as this could lead to undefined behavior. The
     * const version of this function allows for read-only access to the events,
     * ensuring that the caller cannot modify the event data when using a const
     * reference to the EventSet.
     * @return const T* A pointer to the first event in the array, allowing for
     * direct access to the events in the set. The caller should ensure that they
     * do not access beyond the current count of events.
     */
    [[nodiscard]] const T *Data() const { return events_; }

    /**
     * @brief Provides indexed access to the events in the set. This operator
     * allows for retrieving or modifying events at specific indices within the
     * bounds of the current event count. It is important to ensure that the index
     * provided is valid (i.e., less than the current count of events) to avoid
     * undefined behavior. The const version of this operator allows for read-only
     * access to the events, ensuring that the caller cannot modify the event data
     * when using a const reference to the EventSet.
     * @param index The index of the event to access. This value should be less
     * than  the current count of events to ensure valid access.
     * @return T& A reference to the event at the specified index, allowing for
     * retrieval or modification of the event data. The caller should ensure that
     * the index is valid to avoid undefined behavior. The const version of this
     * operator returns a const reference, ensuring read-only access to the event
     * data when using a const reference to the EventSet.
     */
    T &operator[](std::size_t index) { return events_[index]; }

    /**
     * @brief Provides indexed access to the events in the set. This operator
     * allows for retrieving or modifying events at specific indices within the
     * bounds of the current event count. It is important to ensure that the index
     * provided is valid (i.e., less than the current count of events) to avoid
     * undefined behavior. The const version of this operator allows for read-only
     * access to the events, ensuring that the caller cannot modify the event data
     * when using a const reference to the EventSet.
     * @param index The index of the event to access. This value should be less
     * than the current count of events to ensure valid access.
     * @return const T& A reference to the event at the specified index, allowing
     * for retrieval of the event data. The caller should ensure that the index is
     * valid to avoid undefined behavior. The const version of this operator
     * returns a const reference, ensuring read-only access to the event data when
     * using a const reference to the EventSet.
     */
    const T &operator[](std::size_t index) const { return events_[index]; }

    /**
     * @brief Adds a new event to the set if there is available capacity. This
     * function checks if the current event count is less than the defined maximum
     * capacity before adding the new event. If the event is successfully added,
     * it increments the event count and returns true; otherwise, it returns false
     * to indicate that the event could not be added due to reaching capacity. The
     * caller should ensure that the event being added is valid and that the set
     * has not already reached its maximum capacity to avoid failed additions. The
     * Emplace function provides a similar functionality but allows for
     * constructing the event in place using variadic arguments, which can be more
     * efficient for certain types of events.
     * @param event The event to be added to the set. The caller should ensure
     * that this event is valid and that the set has not already reached its
     * maximum capacity to avoid failed additions.
     * @return bool True if the event was successfully added to the set, false if
     * the event could not be added due to reaching capacity.
     */
    bool Add(const T &event)
    {
      if (eventCount_ >= MAX_EVENTS)
      {
        return false;
      }

      events_[eventCount_++] = event;
      return true;
    }

    /**
     * @brief Adds a new event to the set by constructing it in place using
     * variadic arguments. This function checks if there is available capacity
     * before attempting to construct and add the new event. If the event is
     * successfully added, it increments the event count and returns true;
     * otherwise, it returns false to indicate that the event could not be added
     * due to reaching capacity. The Emplace function can be more efficient than
     * Add for certain types of events, as it allows for direct construction of
     * the event within the array without the need for a temporary object. The
     * caller should ensure that the arguments provided are valid for constructing
     * the event type and that the set has not already reached its maximum
     * capacity to avoid failed additions.
     * @tparam Args The types of the arguments required to construct the event.
     * These should be valid for the event type being added to the set.
     * @param args The arguments used to construct the event in place. The caller
     * should ensure that these arguments are valid for constructing the event
     * type and that the set has not already reached its maximum capacity to avoid
     * failed additions.
     * @return bool True if the event was successfully constructed and added to
     * the set, false if the event could not be added due to reaching capacity.
     */
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

    /**
     * @brief Calculates the density of events in the set, defined as the ratio of
     * the number of events that are considered "hits" (i.e., events that have a
     * meaningful value or state) to the total number of events in the set. This
     * function iterates through all events in the set, counting how many are hits
     * based on the IsHit trait defined for the event type. The density is
     * returned as a floating-point value between 0 and 1, where 0 indicates no
     * hits and 1 indicates that all events are hits. This metric can be useful
     * for analyzing the distribution of events in a musical sequence and
     * understanding how densely packed the events are within the set.
     * @return float The density of events in the set, calculated as the ratio of
     * hits to total events. This value is between 0 and 1, where 0 indicates no
     * hits and 1 indicates that all events are hits.
     */
    [[nodiscard]] float GetDensity() const
    {
      int hitCount = 0;
      for (std::size_t i = 0; i < Count(); i++)
      {
        if (IsHit(i))
        {
          hitCount++;
        }
      }
      return static_cast<float>(hitCount) / static_cast<float>(Count());
    }

    /**
     * @brief Provides access to the pitch and value of events in the set based on
     * their index. These functions use the EventTraits defined for the event type
     * to retrieve the pitch and value of the event at the specified index. The
     * caller should ensure that the index provided is valid (i.e., less than the
     * current count of events) to avoid undefined behavior. The IsHit function
     * allows for checking if a specific event at a given index is considered a
     * hit based on its traits, which can be useful for analyzing the events in
     * the set and understanding their significance within a musical sequence.
     * @param index The index of the event for which to retrieve the pitch or
     * value. This value should be less than the current count of events to ensure
     * valid access.
     * @return Note& A reference to the pitch of the event at the specified index,
     * allowing for retrieval or modification of the pitch data. The caller should
     * ensure that the index is valid to avoid undefined behavior. The const
     * version of this function returns a const reference, ensuring read-only
     * access to the pitch data when using a const reference to the EventSet.
     * @return NoteValue& A reference to the value of the event at the specified
     * index, allowing for retrieval or modification of the value data. The caller
     * should ensure that the index is valid to avoid undefined behavior. The
     * const version of this function returns a const reference, ensuring
     * read-only access to the value data when using a const reference to the
     * EventSet.
     * @return bool True if the event at the specified index is considered a hit
     * based on its traits, false otherwise. The caller should ensure that the
     * index is valid to avoid undefined behavior.
     */
    Note &Pitch(std::size_t index)
    {
      return EventTraits<T>::Pitch(events_[index]);
    }

    /**
     * @brief Provides access to the pitch and value of events in the set based on
     * their index. These functions use the EventTraits defined for the event type
     * to retrieve the pitch and value of the event at the specified index. The
     * caller should ensure that the index provided is valid (i.e., less than the
     * current count of events) to avoid undefined behavior. The IsHit function
     * allows for checking if a specific event at a given index is considered a
     * hit based on its traits, which can be useful for analyzing the events in
     * the set and understanding their significance within a musical sequence.
     * @param index The index of the event for which to retrieve the pitch or
     * value. This value should be less than the current count of events to ensure
     * valid access.
     * @return Note& A reference to the pitch of the event at the specified index,
     * allowing for retrieval or modification of the pitch data. The caller should
     * ensure that the index is valid to avoid undefined behavior. The const
     * version of this function returns a const reference, ensuring read-only
     * access to the pitch data when using a const reference to the EventSet.
     */
    [[nodiscard]] const Note &Pitch(std::size_t index) const
    {
      return EventTraits<T>::Pitch(events_[index]);
    }

    /**
     * @brief Provides access to the pitch and value of events in the set based on
     * their index. These functions use the EventTraits defined for the event type
     * to retrieve the pitch and value of the event at the specified index. The
     * caller should ensure that the index provided is valid (i.e., less than the
     * current count of events) to avoid undefined behavior. The IsHit function
     * allows for checking if a specific event at a given index is considered a
     * hit based on its traits, which can be useful for analyzing the events in
     * the set and understanding their significance within a musical sequence.
     * @param index The index of the event for which to retrieve the pitch or
     * value. This value should be less than the current count of events to ensure
     * valid access.
     * @return NoteValue& A reference to the value of the event at the specified
     * index, allowing for retrieval or modification of the value data. The caller
     * should ensure that the index is valid to avoid undefined behavior. The
     * const version of this function returns a const reference, ensuring
     * read-only access to the value data when using a const reference to the
     * EventSet.
     */
    NoteValue &Value(std::size_t index)
    {
      return EventTraits<T>::Value(events_[index]);
    }

    /**
     * @brief Provides access to the pitch and value of events in the set based on
     * their index. These functions use the EventTraits defined for the event type
     * to retrieve the pitch and value of the event at the specified index. The
     * caller should ensure that the index provided is valid (i.e., less than the
     * current count of events) to avoid undefined behavior. The IsHit function
     * allows for checking if a specific event at a given index is considered a
     * hit based on its traits, which can be useful for analyzing the events in
     * the set and understanding their significance within a musical sequence.
     * @param index The index of the event for which to retrieve the pitch or
     * value.  This value should be less than the current count of events to
     * ensure valid access.
     * @return NoteValue& A reference to the value of the event at the specified
     * index, allowing for retrieval of the value data. The caller should ensure
     * that the index is valid to avoid undefined behavior. The const version of
     * this function returns a const reference, ensuring read-only access to the
     * value data when using a const reference to the EventSet.
     */
    [[nodiscard]] const NoteValue &Value(std::size_t index) const
    {
      return EventTraits<T>::Value(events_[index]);
    }

    /**
     * @brief Provides access to the pitch and value of events in the set based on
     * their index. These functions use the EventTraits defined for the event type
     * to retrieve the pitch and value of the event at the specified index. The
     * caller should ensure that the index provided is valid (i.e., less than the
     * current count of events) to avoid undefined behavior. The IsHit function
     * allows for checking if a specific event at a given index is considered a
     * hit based on its traits, which can be useful for analyzing the events in
     * the set and understanding their significance within a musical sequence.
     * @param index The index of the event for which to check if it is a hit. This
     * value should be less than the current count of events to ensure valid
     * access.
     * @return bool True if the event at the specified index is considered a hit
     * based on its traits, false otherwise. The caller should ensure that the
     * index is valid to avoid undefined behavior.
     */
    [[nodiscard]] bool IsHit(std::size_t index) const
    {
      return EventTraits<T>::IsHit(events_[index]);
    }

  private:
    T emptyEvent_{};
    T events_[MAX_EVENTS];
    std::size_t eventCount_{0};
  };

  /**
   * @brief Type aliases for specific event sets, such as PatternEventSet,
   * NoteEventSet, and ChordEventSet, which are defined using the EventSet
   * template with specific event types and maximum capacities. These type aliases
   * provide convenient names for commonly used event sets in musical
   * applications, allowing for easier readability and usage in code that manages
   * musical events. The PatternEventSet is a simple boolean event set that can be
   * used for patterns or rhythms, while the NoteEventSet and ChordEventSet are
   * designed to hold more complex musical events with pitch and value
   * information. The caller should choose the appropriate event set type based on
   * the specific requirements of their musical application.
   */
  template <std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  using PatternEventSet = EventSet<bool, MAX_EVENTS>;

  /**
   * @brief Type aliases for specific event sets, such as PatternEventSet,
   * NoteEventSet, and ChordEventSet, which are defined using the EventSet
   * template with specific event types and maximum capacities. These type aliases
   * provide convenient names for commonly used event sets in musical
   * applications, allowing for easier readability and usage in code that manages
   * musical events. The PatternEventSet is a simple boolean event set that can be
   * used for patterns or rhythms, while the NoteEventSet and ChordEventSet are
   * designed to hold more complex musical events with pitch and value
   * information. The caller should choose the appropriate event set type based on
   * the specific requirements of their musical application.
   */
  template <std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  using NoteEventSet = EventSet<NoteEvent, MAX_EVENTS>;

  /**
   * @brief Type aliases for specific event sets, such as PatternEventSet,
   * NoteEventSet, and ChordEventSet, which are defined using the EventSet
   * template with specific event types and maximum capacities. These type aliases
   * provide convenient names for commonly used event sets in musical
   * applications, allowing for easier readability and usage in code that manages
   * musical events. The PatternEventSet is a simple boolean event set that can be
   * used for patterns or rhythms, while the NoteEventSet and ChordEventSet are
   * designed to hold more complex musical events with pitch and value
   * information. The caller should choose the appropriate event set type based on
   * the specific requirements of their musical application.
   */
  template <std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
            std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
            std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  using ChordEventSet =
      EventSet<ChordEvent<MAX_DEGREES, SCALE_DEGREES>, MAX_EVENTS>;

} // namespace Music
