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
#include <Music/MusicTypes.h>
#include <Music/MusicConst.h>
#include <Music/MusicHelpers.h>
#include <Music/Chord.h>
#include <Music/Note.h>
#include <Music/NoteValue.h>
#include <Music/EventSet.h>
#include <Music/PatternGenerator.h>
#include <Music/NoteGenerator.h>
#include <Music/Persona.h>
#include <Music/PitchEngine.h>
#include <Music/ScaleMap.h>
#include <Music/Tables.h>
#include <Music/Temperament.h>
#include <Music/TimeSignature.h>

#include <cstddef>
#include <cstdint>

namespace Music
{
  ////////////////////////////////////////////////////////////////////////////////
  // Misc. Functions
  ////////////////////////////////////////////////////////////////////////////////
  size_t GenerateChordEventsFromPattern(const PatternEventSet<> &pattern,
                                        NoteValue granularity,
                                        ChordEventSet<> &events);
  size_t GenerateStandardChordEvents(const TimeSignature &ts,
                                     const ScaleMap &scale, int bars,
                                     HarmonicMode mode, NoteValue granularity,
                                     ChordEventSet<> &chords);
  size_t GenerateWeightedChordEvents(const TimeSignature &ts,
                                     const ScaleMap &scale, int bars,
                                     HarmonicMode mode, Note tonic,
                                     NoteValue value, ChordEventSet<> &chords);
  size_t GenerateEventsFromPattern(const PatternEventSet<> &pattern,
                                   const ChordEventSet<> &chords,
                                   const TimeSignature &ts,
                                   const Temperament &temperament,
                                   const ScaleMap &scale, int bars,
                                   NoteValue granularity,
                                   NoteEventSet<> &events);

  ScaleDegree GetWeightedStartingChord(HarmonicMode mode = HarmonicMode::Major);
  ScaleDegree GetWeightedNextChord(ScaleDegree fromDegree,
                                   HarmonicMode mode = HarmonicMode::Major);

  size_t BuildEuclid(int k, int n, int r, PatternEventSet<> &pattern);

  NoteEventScore ScoreNoteEvents(const Temperament &t, const NoteEventSet<> &events);
  NoteValue GetRandomGranularity(NoteValue low, NoteValue high);

  const char *GetNoteValueText(NoteValue v);


#ifndef DAISY_PLATFORM
  void DebugPattern(const TimeSignature &ts, NoteValue granularity,
                    PatternEventSet<> pattern);
  void DebugNoteEvents(const Temperament &t, const TimeSignature &ts,
                       const NoteEventSet<> &events);
  void DebugChordEvents(const TimeSignature &ts, const Temperament &t, const ScaleMap &s, const ChordEventSet<> chords);
#endif

} // namespace Music
