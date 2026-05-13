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
  /// @param weight_map
  /// @param events
  /// @return
  static std::size_t generate_events(
    const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
    const PatternEventSet<MAX_EVENTS>& pattern,
    const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords,
    NoteValue granularity,
    const WeightMap<SCALE_DEGREES>& weight_map,
    NoteEventSet<MAX_EVENTS>& events)
  {
    events.clear();
    if (chords.size() == 0) {
      return 0;
    }

    int pulses = 0;
    int chord_pulses = 0;
    int ppb = setup.time_signature.get_pulses_per_bar();
    std::size_t chord_index = 0;

    std::array<Note, 20> tones;
    std::size_t tone_count = chords[chord_index].get_chord_tones(
      setup.scale_map,
      static_cast<int>(setup.temperament.degrees_per_period()),
      tones);

    for (std::size_t i = 0; i < pattern.size() && !events.at_capacity(); i++,
                     pulses = pulses + granularity,
                     chord_pulses = chord_pulses + granularity) {
      if (chord_pulses >= static_cast<int>(chords[chord_index].value) &&
          chord_index < chords.size() - 1) {
        chord_index++;
        chord_pulses = 0;

        tone_count = chords[chord_index].get_chord_tones(
          setup.scale_map,
          static_cast<int>(setup.temperament.degrees_per_period()),
          tones);
      }

      if (pattern[i]) {
        // Is this at the start of a bar?  Then start with a chord tone
        if ((pulses % ppb == 0) && (random_range(0.0F, 1.0F) < 0.6F)) {
          // Start with a random chord tone.
          int idx = random_range(0, static_cast<int>(tone_count) - 1);
          events.emplace(tones[0], 0, granularity);
        } else {
          int period_offset = 0;
          float unit_random = random_range(0.0F, ALMOST_ONE);

          Note n = setup.scale_map.get_weighted_note(
            unit_random, period_offset, weight_map);

          // If our last event was the same note then (for now) just add
          // to the original duraton Or just a random occurance Unless
          // the new event is at the start of a bar And also unless the
          // new note value leads to something odd (dotted dotted, etc.)
          if ((pulses % ppb != 0) && (events.size() > 1) &&
              ((events[events.size() - 1].note == n) ||
               (random_range(0.0F, 1.0F) < 0.4F)) &&
              !is_note_value_weird(static_cast<NoteValue>(
                events[events.size() - 1].value + granularity))) {
            events[events.size() - 1].value += granularity;
          } else {
            events.emplace(n, period_offset, granularity);
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
    int t = setup.bars * setup.time_signature.get_pulses_per_bar();
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
    const int pulses_per_bar = setup.time_signature.get_pulses_per_bar();
    if (pulses_per_bar <= 0 || setup.bars <= 0 || (setup.bars % 2) != 0) {
      return events.size();
    }

    const int middle_pulse = (setup.bars / 2) * pulses_per_bar;
    if (middle_pulse <= 0 || events.get_total_event_pulses() < middle_pulse) {
      return events.size();
    }

    NoteEvent first_half[MAX_EVENTS];
    std::size_t first_half_count = 0;
    int pulse_cursor = 0;
    for (std::size_t i = 0; i < events.size() && pulse_cursor < middle_pulse;
         i++) {
      NoteEvent event = events[i];
      const int event_pulses = static_cast<int>(event.value);
      const int next_pulse = pulse_cursor + event_pulses;

      if (next_pulse > middle_pulse) {
        event.value = static_cast<NoteValue>(middle_pulse - pulse_cursor);
      }

      first_half[first_half_count++] = event;
      pulse_cursor = next_pulse;
    }

    if (first_half_count > (events.capacity() / 2)) {
      return events.size();
    }

    events.clear();
    for (std::size_t i = 0; i < first_half_count; i++) {
      events.add(first_half[i]);
    }
    for (std::size_t i = 0; i < first_half_count; i++) {
      events.add(first_half[i]);
    }

    return events.size();
  }
};

} // namespace Music
