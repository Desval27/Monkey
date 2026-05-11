/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file note_generator.hpp
 * @brief General Music Definitions.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <cstddef>

#include <music/event_set.hpp>
#include <music/music_setup.hpp>
#include <music/music_tables.hpp>
#include <music/pitch_engine.hpp>

namespace music {

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam MAX_DEGREES
/// @tparam SCALE_DEGREES
/// @tparam MAX_EVENTS
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class NullNoteGenerator
{
public:
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param setup
  /// @param pattern
  /// @param chords
  /// @param granularity
  /// @param events
  /// @return
  static std::size_t generate_pattern(
    const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
    const PatternEventSet<MAX_EVENTS>& pattern,
    const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords,
    NoteValue granularity,
    NoteEventSet<MAX_EVENTS>& events)
  {
    events.clear();
    return events.size();
  }
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam MAX_DEGREES
/// @tparam SCALE_DEGREES
/// @tparam MAX_EVENTS
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class StyleANoteGenerator
{
public:
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param setup
  /// @param pattern
  /// @param chords
  /// @param granularity
  /// @param weightMap
  /// @param events
  /// @return
  static std::size_t generate_events(
    const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
    const PatternEventSet<MAX_EVENTS>& pattern,
    const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords,
    NoteValue granularity,
    const WeightMap<SCALE_DEGREES>& weightMap,
    NoteEventSet<MAX_EVENTS>& events)
  {
    events.clear();
    if (chords.size() == 0) {
      return 0;
    }

    int pulses = 0;
    int chordPulses = 0;
    int ppb = setup.timeSignature.get_pulses_per_bar();
    std::size_t chordIdx = 0;

    Note tones[20];
    std::size_t toneCount = chords[chordIdx].get_chord_tones(
      setup.scaleMap,
      static_cast<int>(setup.temperament.degrees_per_period()),
      tones,
      ArrayLen(tones));

    for (std::size_t i = 0; i < pattern.size() && !events.at_capacity(); i++,
                     pulses = pulses + granularity,
                     chordPulses = chordPulses + granularity) {
      if (chordPulses >= static_cast<int>(chords[chordIdx].value) &&
          chordIdx < chords.size() - 1) {
        chordIdx++;
        chordPulses = 0;

        toneCount = chords[chordIdx].get_chord_tones(
          setup.scaleMap,
          static_cast<int>(setup.temperament.degrees_per_period()),
          tones,
          ArrayLen(tones));
      }

      if (pattern[i]) {
        // Is this at the start of a bar?  Then start with a chord tone
        if ((pulses % ppb == 0) && (randomRange(0.0F, 1.0F) < 0.6F)) {
          // Start with a random chord tone.
          int idx = randomRange(0, static_cast<int>(toneCount) - 1);
          events.emplace(tones[0], 0, granularity);
        } else {
          int periodOffset = 0;
          float unitRandom = randomRange(0.0F, 0.999999F);

          Note n =
            setup.scaleMap.GetWeightedNote(unitRandom, periodOffset, weightMap);

          // If our last event was the same note then (for now) just add
          // to the original duraton Or just a random occurance Unless
          // the new event is at the start of a bar And also unless the
          // new note value leads to something odd (dotted dotted, etc.)
          if ((pulses % ppb != 0) && (events.size() > 1) &&
              ((events[events.size() - 1].note == n) ||
               (randomRange(0.0F, 1.0F) < 0.4F)) &&
              !is_note_value_weird(static_cast<NoteValue>(
                events[events.size() - 1].value + granularity))) {
            events[events.size() - 1].value += granularity;
          } else {
            events.emplace(n, periodOffset, granularity);
          }
        }
      } else {
        // If our last event was also a rest then just add to its value
        // Unless the new event is at the start of a bar
        if ((pulses % ppb != 0 && events.size() > 1) &&
            (events[events.size() - 1].note == REST) &&
            !is_note_value_weird(static_cast<NoteValue>(
              events[events.size() - 1].value + granularity))) {
          events[events.size() - 1].value += granularity;
        } else {
          events.emplace(REST, 0, granularity);
        }
      }
    }

    // Do we have any remainders?  If so then create a single rest
    // of that value if we have any additional event capacity.
    int t = setup.bars * setup.timeSignature.get_pulses_per_bar();
    int r = t - events.get_total_event_pulses();
    if (r > 0) {
      // Do we need to add a new event or just tack on to the last one
      // if it is already a rest?
      if (events.size() > 0 && events[events.size() - 1].note == REST) {
        events[events.size() - 1].value += r;
      } else if (!events.at_capacity()) {
        events.emplace(REST, 0, static_cast<NoteValue>(r));
      }
      // Otherwise we're just goint to lose it for now.
    }
    return events.size();
  }
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam MAX_DEGREES
/// @tparam SCALE_DEGREES
/// @tparam MAX_EVENTS
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class PatternModifier
{
public:
  static std::size_t repeat_two(const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
                                NoteEventSet<MAX_EVENTS>& events)
  {
#ifdef USE_DEBUG
    std::cout << __FILE__ << ":" << __LINE__ << " - " << __func__ << "\n";
#endif
    const int pulsesPerBar = setup.timeSignature.get_pulses_per_bar();
    if (pulsesPerBar <= 0 || setup.bars <= 0 || (setup.bars % 2) != 0) {
      return events.size();
    }

    const int middlePulse = (setup.bars / 2) * pulsesPerBar;
    if (middlePulse <= 0 || events.get_total_event_pulses() < middlePulse) {
      return events.size();
    }

    NoteEvent firstHalf[MAX_EVENTS];
    std::size_t firstHalfCount = 0;
    int pulseCursor = 0;
    for (std::size_t i = 0; i < events.size() && pulseCursor < middlePulse;
         i++) {
      NoteEvent event = events[i];
      const int eventPulses = static_cast<int>(event.value);
      const int nextPulse = pulseCursor + eventPulses;

      if (nextPulse > middlePulse) {
        event.value = static_cast<NoteValue>(middlePulse - pulseCursor);
      }

      firstHalf[firstHalfCount++] = event;
      pulseCursor = nextPulse;
    }

    if (firstHalfCount > (events.capacity() / 2)) {
      return events.size();
    }

    events.clear();
    for (std::size_t i = 0; i < firstHalfCount; i++) {
      events.add(firstHalf[i]);
    }
    for (std::size_t i = 0; i < firstHalfCount; i++) {
      events.add(firstHalf[i]);
    }

    return events.size();
  }
};

} // namespace Music
