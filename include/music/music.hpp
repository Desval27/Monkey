/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file music.hpp
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
#include <cstdint>

#include <music/event_set.hpp>
#include <music/music_setup.hpp>

namespace music {

////////////////////////////////////////////////////////////////////////////////
// Misc. Functions
////////////////////////////////////////////////////////////////////////////////
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
std::size_t
GenerateChordEventsFromPattern(
  const PatternEventSet<MAX_EVENTS>& pattern,
  NoteValue granularity,
  ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& events);

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
std::size_t
GenerateStandardChordEvents(
  const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
  NoteValue granularity,
  ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords);

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_DEGREES>
std::size_t
GenerateWeightedChordEvents(
  const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
  Note tonic,
  NoteValue granularity,
  ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords);

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
std::size_t
GenerateEventsFromPattern(
  const PatternEventSet<MAX_EVENTS>& pattern,
  const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords,
  const TimeSignature& ts,
  const Temperament<MAX_DEGREES>& temperament,
  const ScaleMap<SCALE_DEGREES>& scale,
  int bars,
  NoteValue granularity,
  NoteEventSet<MAX_EVENTS>& events);

ScaleDegree
get_weighted_starting_chord(HarmonicMode mode = HarmonicMode::Major);
ScaleDegree
get_weighted_next_chord(ScaleDegree fromDegree,
                        HarmonicMode mode = HarmonicMode::Major);

template<std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
std::size_t
build_euclid(int k, int n, int r, PatternEventSet<MAX_EVENTS>& pattern);

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
NoteEventScore
ScoreNoteEvents(const Temperament<MAX_DEGREES>& t,
                const NoteEventSet<MAX_EVENTS>& events);

NoteValue
get_random_granularity(NoteValue low, NoteValue high);

const char*
get_note_value_text(NoteValue v);

#ifdef USE_DEBUG
template<std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
void
DebugPattern(const TimeSignature& ts,
             NoteValue granularity,
             const PatternEventSet<MAX_EVENTS>& pattern);

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
void
DebugNoteEvents(const Temperament<MAX_DEGREES>& t,
                const TimeSignature& ts,
                const NoteEventSet<MAX_EVENTS>& events);

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
void
DebugChordEvents(
  const Setup<MAX_DEGREES, SCALE_DEGREES>& setup,
  const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords);
#endif

} // namespace Music

#include <music/music_templates.hpp>
