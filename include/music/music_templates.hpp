/* SPDX-License-Identifier: CC0-1.0 */
#pragma once

#ifdef USE_DEBUG
#include <iomanip>
#include <iostream>
#include <tty.h>
#endif

#include <Music/music.hpp>
#include <Music/note_generator.hpp>

namespace music {
template<std::size_t MAX_DEGREES,
         std::size_t SCALE_DEGREES,
         std::size_t MAX_EVENTS>
std::size_t
GenerateWeightedChordEvents(
  const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
  Note tonic,
  NoteValue granularity,
  ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords)
{
  if (chords.capacity() == 0 || setup.bars <= 0 ||
      granularity == NoteValue::None) {
    return 0;
  }

  const std::size_t totalEvents =
    (static_cast<std::size_t>(setup.bars) *
     static_cast<std::size_t>(setup.time_signature.beats) *
     static_cast<std::size_t>(setup.time_signature.beatValue)) /
    static_cast<std::size_t>(granularity);
  const std::size_t chordsToEmit = min(totalEvents, chords.capacity());
  ScaleDegree degree =
    get_weighted_starting_chord(setup.scale_map.get_harmonic_mode());
  const bool hasScale = (setup.scale_map.size() >= SCALE_CHORD_COUNT);

  chords.clear();
  for (std::size_t i = 0; i < chordsToEmit; ++i) {
    const int degreeIndex =
      scale_degree_index(degree, setup.scale_map.get_harmonic_mode());
    Note rootOffset = scale_degree_to_semitone(degree);
    if (hasScale && degreeIndex >= 0) {
      int scalePeriodOffset = 0;
      rootOffset = static_cast<Note>(
        setup.scale_map.get_mapped_degree(degreeIndex, scalePeriodOffset));
    }

    ChordExtension extensions =
      (degree == ScaleDegree::V) ? ChordExtension::Seventh : ChordExtension{};
    chords.emplace(tonic + rootOffset, granularity, extensions);

    degree =
      get_weighted_next_chord(degree, setup.scale_map.get_harmonic_mode());
  }
  return chords.size();
}

template<std::size_t MAX_DEGREES,
         std::size_t SCALE_DEGREES,
         std::size_t MAX_EVENTS>
std::size_t
GenerateStandardChordEvents(
  const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
  NoteValue granularity,
  ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords)
{
  return GenerateWeightedChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(
    setup, 0, granularity, chords);
}

template<std::size_t MAX_DEGREES,
         std::size_t SCALE_DEGREES,
         std::size_t MAX_EVENTS>
std::size_t
GenerateEventsFromPattern(
  const PatternEventSet<MAX_EVENTS>& pattern,
  const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords,
  const TimeSignature& ts,
  const Temperament<MAX_DEGREES>& temperament,
  const ScaleMap<SCALE_DEGREES>& scale,
  int bars,
  NoteValue granularity,
  NoteEventSet<MAX_EVENTS>& events)
{
  (void)bars;
  events.clear();
  if (chords.size() == 0) {
    return 0;
  }

  int pulses = 0;
  int chordPulses = 0;
  int ppb = ts.get_pulses_per_bar();
  std::size_t chordIdx = 0;

  std::array<Note, 20> tones;
  std::size_t toneCount = chords[chordIdx].get_chord_tones(
    scale, static_cast<int>(temperament.degrees_per_period()), tones);

  for (std::size_t i = 0; i < pattern.size() && !events.at_capacity(); i++,
                   pulses = pulses + granularity,
                   chordPulses = chordPulses + granularity) {
    if (chordPulses >= static_cast<int>(chords[chordIdx].value) &&
        chordIdx < chords.size() - 1) {
      chordIdx++;
      chordPulses = 0;

      toneCount = chords[chordIdx].get_chord_tones(
        scale, static_cast<int>(temperament.degrees_per_period()), tones);
    }

    if (pattern[i]) {
      if (pulses % ppb == 0) {
        events.emplace(tones[0], 0, granularity);
      } else {
        int idx = randomRange(0, static_cast<int>(toneCount) - 1);
        events.emplace(tones[idx], 0, granularity);
      }
    } else {
      if (pulses % ppb != 0 && events.size() > 1 &&
          events[events.size() - 1].note == REST) {
        events[events.size() - 1].value += granularity;
      } else {
        events.emplace(REST, 0, granularity);
      }
    }
  }
  return events.size();
}

template<std::size_t MAX_DEGREES, std::size_t MAX_EVENTS>
NoteEventScore
ScoreNoteEvents(const Temperament<MAX_DEGREES>& t,
                const NoteEventSet<MAX_EVENTS>& events)
{
  NoteEventScore score;
  if (events.size() == 0) {
    return score;
  }

  int total_pulses = 0;
  int pitched_pulses = 0;
  int rest_pulses = 0;
  int pitched_event_count = 0;
  int short_pitched_events = 0;
  int long_pitched_events = 0;
  int repeated_notes = 0;
  int repeated_durations = 0;
  int phrase_count = 0;
  int consonant_phrase_starts = 0;
  int consonant_phrase_ends = 0;
  int step_moves = 0;
  int huge_leaps = 0;
  int transition_count = 0;

  const int long_threshold = static_cast<int>(NoteValue::Half);
  const int short_threshold = static_cast<int>(NoteValue::Eighth);

  Note pervious_pitched_note = REST;
  NoteValue previous_pitched_duration = NoteValue::None;
  bool in_phrase = false;

  for (std::size_t i = 0; i < events.size(); i++) {
    int event_pulses = static_cast<int>(events[i].value);
    total_pulses += event_pulses;

    if (!events[i].IsPitched()) {
      rest_pulses += event_pulses;
      in_phrase = false;
      continue;
    }

    pitched_pulses += event_pulses;
    pitched_event_count++;

    if (!in_phrase) {
      phrase_count++;
      if (t.is_consonant(t.get_note_distance(0, events[i].note))) {
        consonant_phrase_starts++;
      }
      in_phrase = true;
    }

    if (event_pulses <= short_threshold) {
      short_pitched_events++;
    }
    if (event_pulses >= long_threshold) {
      long_pitched_events++;
    }

    if (pervious_pitched_note != REST) {
      const int interval =
        t.get_note_distance(pervious_pitched_note, events[i].note);
      if (events[i].note == pervious_pitched_note) {
        repeated_notes++;
      } else if (interval <= 2) {
        step_moves++;
      } else if (interval >= 7) {
        if (interval >= 10) {
          huge_leaps++;
        }
      }

      if (events[i].value == previous_pitched_duration) {
        repeated_durations++;
      }

      transition_count++;
    }

    const bool next_starts_new_phrase = (i + 1 >= events.size()) ||
                                        !events[i + 1].IsPitched() ||
                                        event_pulses >= long_threshold;

    if (next_starts_new_phrase &&
        t.is_consonant(t.get_note_distance(0, events[i].note))) {
      consonant_phrase_ends++;
    }

    pervious_pitched_note = events[i].note;
    previous_pitched_duration = events[i].value;
  }

  if (pitched_pulses == 0 || total_pulses == 0) {
    return score;
  }

  const int density_pct =
    static_cast<int>((pitched_pulses * 100UL) / total_pulses);
  const int rest_pct = static_cast<int>((rest_pulses * 100UL) / total_pulses);
  const int phrase_start_pct =
    phrase_count > 0
      ? static_cast<int>((consonant_phrase_starts * 100UL) / phrase_count)
      : 0;
  const int phrase_end_pct =
    phrase_count > 0
      ? static_cast<int>((consonant_phrase_ends * 100UL) / phrase_count)
      : 0;
  const int repeat_pct =
    transition_count > 0
      ? static_cast<int>(((repeated_notes + repeated_durations) * 100UL) /
                         (transition_count * 2UL))
      : 0;
  const int step_pct =
    transition_count > 0
      ? static_cast<int>((step_moves * 100UL) / transition_count)
      : 0;
  const int huge_leap_pct =
    transition_count > 0
      ? static_cast<int>((huge_leaps * 100UL) / transition_count)
      : 0;
  const int phrase_density =
    pitched_event_count > 0
      ? static_cast<int>((phrase_count * 100UL) / pitched_event_count)
      : 0;
  const int rhythmic_contrast =
    (short_pitched_events + long_pitched_events) > 0
      ? static_cast<int>(
          (min(short_pitched_events, long_pitched_events) * 200UL) /
          (short_pitched_events + long_pitched_events))
      : 0;

  score.density = clamp(100 - (abs(density_pct - 55) * 2), 0, 100);
  score.rests = clamp(100 - (abs(rest_pct - 25) * 2), 0, 100);
  score.cadence = clamp((phrase_start_pct + (phrase_end_pct * 2)) / 3, 0, 100);
  score.repetition = clamp(100 - (abs(repeat_pct - 30) * 2), 0, 100);
  score.contour = clamp(100 - abs(step_pct - 65) - (huge_leap_pct * 2), 0, 100);
  score.phrase = clamp(100 - (abs(phrase_density - 35) * 2), 0, 100);
  score.rhythm = clamp(rhythmic_contrast, 0, 100);

  score.overall =
    ((score.density * 20U) + (score.rests * 10U) + (score.cadence * 25U) +
     (score.repetition * 15U) + (score.contour * 15U) + (score.phrase * 10U) +
     (score.rhythm * 5U)) /
    100U;

  return score;
}

#ifdef USE_DEBUG
template<std::size_t MAX_EVENTS>
void
DebugPattern(const TimeSignature& ts,
             NoteValue granularity,
             const PatternEventSet<MAX_EVENTS>& pattern)
{
  std::cout << TTY_FG_YELLOW << "PATTERN EVENTS" << TTY_RESET << "\t"
            << ts.beats << "/" << ts.get_denominator()
            << "\tDensity: " << pattern.get_density() << std::endl;
  if (pattern.size() == 0 || granularity == NoteValue::None) {
    return;
  }

  const int g = static_cast<int>(granularity);
  const int kPerBar = (ts.beats * static_cast<int>(ts.beatValue)) / g;
  if (kPerBar > 0) {
    for (std::size_t i = 0; i < pattern.size(); i++) {
      if (i % kPerBar == 0) {
        std::cout << "|";
      }
      std::cout << (pattern[i] ? "*" : "-");
    }
  }
  std::cout << "|" << '\n';
}

template<std::size_t MAX_DEGREES, std::size_t MAX_EVENTS>
void
DebugNoteEvents(const Temperament<MAX_DEGREES>& t,
                const TimeSignature& ts,
                const NoteEventSet<MAX_EVENTS>& events)
{
  std::cout << TTY_FG_CYAN << "NOTE EVENTS" << TTY_RESET
            << " Density: " << events.get_density() << std::endl;

  int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
  int v = 0;
  MString<8> note_label;
  for (std::size_t i = 0; i < events.size(); i++) {
    if (!events[i].IsPitched()) {
      std::cout << TTY_FAINT;
    }

    t.get_note_label(events[i].note, note_label);
    std::cout << std::right << std::setw(6) << note_label
              << static_cast<int>(events[i].period) << "-" << std::left
              << std::setw(6) << get_note_value_text(events[i].value);

    if (!events[i].IsPitched()) {
      std::cout << TTY_RESET;
    }

    v = events[i].value + v;
    if (v % vPerBar == 0) {
      std::cout << '\n';
    }
  }
}

template<std::size_t MAX_DEGREES,
         std::size_t SCALE_DEGREES,
         std::size_t MAX_EVENTS>
void
DebugChordEvents(
  const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
  const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords)
{
  int vPerBar = setup.time_signature.beats *
                static_cast<int>(setup.time_signature.beatValue);
  int v = 0;
  char note_label[8];
  MString<16> chordName;
  std::cout << TTY_FG_GREEN << "CHORD EVENTS" << TTY_RESET
            << " Density: " << chords.get_density() << std::endl;
  for (std::size_t i = 0; i < chords.size(); i++) {
    std::array<Note, 8> tones;
    std::size_t toneCount = chords[i].get_chord_tones(
      setup.scale_map,
      static_cast<int>(setup.temperament.degrees_per_period()),
      tones);

    if (v % vPerBar == 0) {
      std::cout << "| ";
    }
    chords[i].get_chord_name(setup.scale_map, setup.temperament, chordName);
    std::cout << chordName << " (";

    for (std::size_t j = 0; j < toneCount; j++) {
      setup.temperament.get_note_label(
        tones[j], note_label, sizeof(note_label));
      std::cout << note_label;
      if (j < toneCount - 1) {
        std::cout << "|";
      }
    }

    std::cout << ")-" << get_note_value_text(chords[i].value) << " ";
    v = chords[i].value + v;
  }
  std::cout << "|" << '\n';
}
#endif

} // namespace Music
