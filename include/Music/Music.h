/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Music.h
 * @brief General Music Definitions.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Monkey.h>
#include <Music/Chord.h>
#include <Music/EventSet.h>
#include <Music/MusicConfig.h>
#include <Music/MusicConst.h>
#include <Music/MusicHelpers.h>
#include <Music/MusicTypes.h>
#include <Music/Note.h>
#include <Music/NoteGenerator.h>
#include <Music/NoteValue.h>
#include <Music/PatternGenerator.h>
#include <Music/Persona.h>
#include <Music/PitchEngine.h>
#include <Music/ScaleMap.h>
#include <Music/Setup.h>
#include <Music/Tables.h>
#include <Music/Temperament.h>
#include <Music/TimeSignature.h>

#include <cstddef>
#include <cstdint>

namespace Music {
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
GetWeightedStartingChord(HarmonicMode mode = HarmonicMode::Major);
ScaleDegree
GetWeightedNextChord(ScaleDegree fromDegree,
                     HarmonicMode mode = HarmonicMode::Major);

template<std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
std::size_t
BuildEuclid(int k, int n, int r, PatternEventSet<MAX_EVENTS>& pattern);

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
NoteEventScore
ScoreNoteEvents(const Temperament<MAX_DEGREES>& t,
                const NoteEventSet<MAX_EVENTS>& events);

NoteValue
GetRandomGranularity(NoteValue low, NoteValue high);

const char*
GetNoteValueText(NoteValue v);

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

#include <Music/MusicTemplates.h>