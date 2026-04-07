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

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "MusicTypes.h"
#include "MusicConst.h"
#include "MusicHelpers.h"
#include "Temperament.h"
#include "ScaleMap.h"
#include "PitchEngine.h"
#include "TimeSignature.h"
#include "Note.h"
#include "Chord.h"

namespace Music
{
    
////////////////////////////////////////////////////////////////////////////////
// Misc. Functions
////////////////////////////////////////////////////////////////////////////////
extern size_t      generateStandardChordEvents(const TimeSignature &ts,
                                               const ScaleMap      &scale,
                                               int                  bars,
                                               HarmonicMode         mode,
                                               NoteValue            granularity,
                                               ChordEvent          *eventsOut,
                                               size_t               eventMax);
extern size_t      generateWeightedChordEvents(const TimeSignature &ts,
                                               const ScaleMap      &scale,
                                               int                  bars,
                                               HarmonicMode         mode,
                                               Note                 tonic,
                                               NoteValue            value,
                                               ChordEvent          *eventsOut,
                                               size_t               eventMax);
extern ScaleDegree getWeightedStartingChord(HarmonicMode mode
                                            = HarmonicMode::Major);
extern ScaleDegree getWeightedNextChord(ScaleDegree  fromDegree,
                                        HarmonicMode mode
                                        = HarmonicMode::Major);
extern size_t      generateChordEventsFromPattern(const bool *pattern,
                                                  size_t      patternSize,
                                                  NoteValue   granularity,
                                                  ChordEvent *eventsOut,
                                                  size_t      eventsMax);
extern size_t      generatePattern(const TimeSignature &ts,
                                   int                  bars,
                                   float                density,
                                   NoteValue            granularity,
                                   bool                *out,
                                   size_t               outSize);
extern size_t      buildEuclid(int k, int n, int r, bool *out, size_t outMax);
#if USE_DEBUG
extern void debugPattern(const TimeSignature &ts,
                         NoteValue            granularity,
                         const bool          *pattern,
                         size_t               patternSize);
extern void debugChordEvents(const Temperament   &t,
                             const TimeSignature &ts,
                             const ChordEvent    *events,
                             size_t               eventsSize);
extern void debugNoteEvents(const Temperament   &t,
                            const TimeSignature &ts,
                            const NoteEvent     *events,
                            size_t               eventsSize);
#endif

} // namespace Music
