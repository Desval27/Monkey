/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file ScaleMaps.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief Musical Scale Maps based on degrees
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
*/
#pragma once

#include <stdint.h>

namespace Music
{
// Modes of the Major Scale
static const Degree IONIAN_DEGREES_12[]            = {0, 2, 4, 5, 7, 9, 11};
static const Degree DORIAN_DEGREES_12[]            = {0, 2, 3, 5, 7, 9, 10};
static const Degree PHRYGIAN_DEGREES_12[]          = {0, 1, 3, 5, 7, 8, 10};
static const Degree PHRYGIAN_DOMINANT_DEGREES_12[] = {0, 1, 4, 5, 7, 8, 10};
static const Degree LYDIAN_DEGREES_12[]            = {0, 2, 4, 6, 7, 9, 11};
static const Degree MIXOLYDIAN_DEGREES_12[]        = {0, 2, 4, 5, 7, 9, 10};
static const Degree AEOLIAN_DEGREES_12[]           = {0, 2, 3, 5, 7, 8, 10};
static const Degree LOCRIAN_DEGREES_12[]           = {0, 1, 3, 5, 6, 8, 10};
static const Degree HUNGARIAN_MINOR_DEGREES_12[]   = {0, 2, 3, 6, 7, 8, 11};
static const Degree DOUBLE_HARMONIC_MAJOR_DEGREES_12[]
    = {0, 1, 4, 5, 7, 8, 11};

// Common pentatonic maps in 12-EDO.
static const Degree MAJOR_PENTATONIC_DEGREES_12[]     = {0, 2, 4, 7, 9};
static const Degree MINOR_PENTATONIC_DEGREES_12[]     = {0, 3, 5, 7, 10};
static const Degree SUSPENDED_PENTATONIC_DEGREES_12[] = {0, 2, 5, 7, 10};

// Common hexatonic maps in 12-EDO.
static const Degree WHOLE_TONE_HEXATONIC_DEGREES_12[] = {0, 2, 4, 6, 8, 10};
static const Degree BLUES_HEXATONIC_DEGREES_12[]      = {0, 3, 5, 6, 7, 10};
static const Degree AUGMENTED_HEXATONIC_DEGREES_12[]  = {0, 3, 4, 7, 8, 11};

// Reusable 7-note scale weight presets.
// Index 0 maps to the first degree in the selected scale degree map.
// These are relative values; they do not need to sum to 1.0f.
static const float SCALE_WEIGHTS_7_UNIFORM[]
    = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const float SCALE_WEIGHTS_7_TONIC_HEAVY[]
    = {5.0f, 1.0f, 1.5f, 1.0f, 3.0f, 1.0f, 1.5f};
static const float SCALE_WEIGHTS_7_CHORD_TONE_HEAVY[]
    = {4.0f, 0.75f, 3.0f, 0.75f, 3.5f, 0.75f, 1.5f};

// Reusable 6-note scale weight presets.
static const float SCALE_WEIGHTS_6_UNIFORM[]
    = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const float SCALE_WEIGHTS_6_TONIC_HEAVY[]
    = {5.0f, 1.0f, 1.5f, 1.0f, 3.0f, 1.25f};
static const float SCALE_WEIGHTS_6_CHORD_TONE_HEAVY[]
    = {4.0f, 0.8f, 3.0f, 1.0f, 3.5f, 1.25f};

// Reusable 5-note scale weight presets.
static const float SCALE_WEIGHTS_5_UNIFORM[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const float SCALE_WEIGHTS_5_TONIC_HEAVY[]
    = {5.0f, 1.25f, 1.75f, 3.5f, 1.5f};
static const float SCALE_WEIGHTS_5_CHORD_TONE_HEAVY[]
    = {4.0f, 1.0f, 3.0f, 3.5f, 1.5f};

static_assert(ArrayLen(IONIAN_DEGREES_12) == 7, "IONIAN_DEGREES_12 array length mismatch");


} // namespace Music
