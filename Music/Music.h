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

#include "MusicTypes.h"
#include "MusicConst.h"
#include "MusicHelpers.h"
#include "Tables.h"
#include "Temperament.h"
#include "ScaleMap.h"
#include "PitchEngine.h"
#include "TimeSignature.h"
#include "Note.h"
#include "Chord.h"
#include "EventSet.h"

namespace Music
{
    
////////////////////////////////////////////////////////////////////////////////
// Misc. Functions
////////////////////////////////////////////////////////////////////////////////
extern size_t      GenerateStandardChordEvents(const TimeSignature &ts,
                                               const ScaleMap      &scale,
                                               int                  bars,
                                               HarmonicMode         mode,
                                               NoteValue            granularity,
                                               ChordEvent          *eventsOut,
                                               size_t               eventMax);
extern size_t      GenerateWeightedChordEvents(const TimeSignature &ts,
                                               const ScaleMap      &scale,
                                               int                  bars,
                                               HarmonicMode         mode,
                                               Note                 tonic,
                                               NoteValue            value,
                                               ChordEvent          *eventsOut,
                                               size_t               eventMax);
extern ScaleDegree GetWeightedStartingChord(HarmonicMode mode
                                            = HarmonicMode::Major);
extern ScaleDegree GetWeightedNextChord(ScaleDegree  fromDegree,
                                        HarmonicMode mode
                                        = HarmonicMode::Major);
extern size_t      GenerateChordEventsFromPattern(const bool *pattern,
                                                  size_t      patternSize,
                                                  NoteValue   granularity,
                                                  ChordEvent *eventsOut,
                                                  size_t      eventsMax);
extern size_t      GeneratePattern(const TimeSignature &ts,
                                   int                  bars,
                                   float                density,
                                   NoteValue            granularity,
                                   bool                *out,
                                   size_t               outSize);
extern size_t      BuildEuclid(int k, int n, int r, bool *out, size_t outMax);
#if USE_DEBUG
extern void DebugPattern(const TimeSignature &ts,
                         NoteValue            granularity,
                         const bool          *pattern,
                         size_t               patternSize);
extern void DebugChordEvents(const Temperament   &t,
                             const TimeSignature &ts,
                             const ChordEvent    *events,
                             size_t               eventsSize);
extern void DebugNoteEvents(const Temperament   &t,
                            const TimeSignature &ts,
                            const NoteEvent     *events,
                            size_t               eventsSize);
#endif

} // namespace Music
