/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Tables.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#ifdef DAISY_PLATFORM
#include <daisy_seed.h>
#define MEMORY_SECTION 

#else

#ifdef ARDUINO_PLATFORM
#define MEMORY_SECTION PROGMEM
#else

// For non-embedded platforms, we don't need special memory attributes. 
#define MEMORY_SECTION
#endif
#endif

#include <Monkey.h>
#include "Chord.h"

#include <Music/Tone12.h>

namespace Music
{
    // clang-format off

    // Note Names
    static const char *n_C = "C";
    static const char *n_Cs = "C#";
    static const char *n_Cs_Db = "C#/Db";
    static const char *n_Db = "Db";
    static const char *n_D = "D";
    static const char *n_Ds = "D#";
    static const char *n_Ds_Eb = "D#/Eb";
    static const char *n_Eb = "Eb";
    static const char *n_E = "E";
    static const char *n_Es_Fb = "E#/Fb";
    static const char *n_F = "F";
    static const char *n_Fs = "F#";
    static const char *n_Fs_Gb = "F#/Gb";
    static const char *n_Gb = "Gb";
    static const char *n_G = "G";
    static const char *n_Gs = "G#";
    static const char *n_Gs_Ab = "G#/Ab";
    static const char *n_Ab = "Ab";
    static const char *n_A = "A";
    static const char *n_As = "A#";
    static const char *n_As_Bb = "A#/Bb";
    static const char *n_Bb = "Bb";
    static const char *n_B = "B";
    static const char *n_Bs = "B#";
    static const char *n_Bs_Cb = "B#/Cb";
    static const char *n_Bs_C = "B#/C";
    static const char *n_Cb = "Cb";
    static const char *n_Cb_B = "Cb/B";

    // Interval Names
    static const char *i_P1 = "P1";
    static const char *i_A1 = "A1";
    static const char *i_m2 = "m2";
    static const char *i_M2 = "M2";
    static const char *i_A2 = "A2";
    static const char *i_m3 = "m3";
    static const char *i_M3 = "M3";
    static const char *i_A3 = "A3";
    static const char *i_P4 = "P4";
    static const char *i_A4 = "A4";
    static const char *i_TT = "TT";
    static const char *i_d5 = "d5";
    static const char *i_P5 = "P5";
    static const char *i_A5 = "A5";
    static const char *i_m6 = "m6";
    static const char *i_M6 = "M6";
    static const char *i_A6 = "A6";
    static const char *i_m7 = "m7";
    static const char *i_M7 = "M7";
    static const char *i_A7 = "A7";

    //
    // 12 Tone Temperaments
    //
    static const char *const NOTE_NAMES_12[] = {
        n_C,
        n_Cs_Db,
        n_D,
        n_Ds_Eb,
        n_E,
        n_F,
        n_Fs_Gb,
        n_G,
        n_Gs_Ab,
        n_A,
        n_As_Bb,
        n_B};

    static const char *const INTERVAL_NAMES_12[] = {
        i_P1,
        i_m2,
        i_M2,
        i_m3,
        i_M3,
        i_P4,
        i_TT,
        i_P5,
        i_m6,
        i_M6,
        i_m7,
        i_M7};

    //
    // 15 Tone Temperaments
    //
    static const char *const NOTE_NAMES_15[] = {
        // 0        1    2     3     4     5     6         7    8     9    10   11    12    13   14
         n_C, n_Cs_Db, n_D, n_Ds, n_Eb,  n_E,  n_F,  n_Fs_Gb, n_G, n_Gs, n_Ab, n_A, n_As, n_Bb, n_B
    };

    static const char *const INTERVAL_NAMES_15[] = {
        // 0     1     2     3     4     5     6     7     8     9    10    11    12    13    14
        i_P1, i_m2, i_M2, i_A2, i_m3, i_M3, i_P4, i_TT, i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7
    };

    //
    // 17 Tone Temperaments
    //
    static const char *const NOTE_NAMES_17[] = {
        n_C,
        n_Cs,
        n_Db,
        n_D,
        n_Ds,
        n_Eb,
        n_E,
        n_F,
        n_Fs,
        n_Gb,
        n_G,
        n_Gs,
        n_Ab,
        n_A,
        n_As,
        n_Bb,
        n_B};

    static const char *const INTERVAL_NAMES_17[] = {
        i_P1,
        i_A1,
        i_m2,
        i_M2,
        i_A2,
        i_m3,
        i_M3,
        i_P4,
        i_A4,
        i_d5,
        i_P5,
        i_A5,
        i_m6,
        i_M6,
        i_A6,
        i_m7,
        i_M7};

    //
    // 19 Tone Temperaments
    //
    static const char *const NOTE_NAMES_19[] = {
        n_C,
        n_Cs,
        n_Db,
        n_D,
        n_Ds,
        n_Eb,
        n_E,
        n_Es_Fb,
        n_F,
        n_Fs,
        n_Gb,
        n_G,
        n_Gs,
        n_Ab,
        n_A,
        n_As,
        n_Bb,
        n_B,
        n_Bs_Cb};

    static const char *const INTERVAL_NAMES_19[] = {
        i_P1,
        i_A1,
        i_m2,
        i_M2,
        i_A2,
        i_m3,
        i_M3,
        i_A3,
        i_P4,
        i_A4,
        i_d5,
        i_P5,
        i_A5,
        i_m6,
        i_M6,
        i_A6,
        i_m7,
        i_M7,
        i_A7};


static const char *s_IONIAN = "IONIAN";
static const char *s_DORIAN = "DORIAN";
static const char *s_PHRYGIAN = "PHRYGIAN";
static const char *s_LYDIAN = "LYDIAN";
static const char *s_MIXOLYDIAN = "MIXOLYDIAN";
static const char *s_AEOLIAN = "AEOLIAN";
static const char *s_LOCRIAN = "LOCRIAN";



// Modes of the Diatonic Scale in 15 Tone Temperament.
//    0     1     2     3     4     5     6     7     8     9    10    11    12    13    14
// i_P1, i_m2, i_M2, i_A2, i_m3, i_M3, i_P4, i_TT, i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7};
static const Degree IONIAN_D15[]            = {0, 2, 5, 6, 8, 11, 14}; // P1, M2, M3, P4, P5, M6, M7
static const Degree DORIAN_D15[]            = {0, 2, 4, 6, 8, 11, 13}; // P1, M2, m3, P4, P5, M6, m7
static const Degree PHRYGIAN_D15[]          = {0, 1, 4, 6, 8, 10, 13}; // P1, m2, m3, P4, P5, m6, m7
static const Degree LYDIAN_D15[]            = {0, 2, 5, 7, 8, 11, 14}; // P1, M2, M3, A4, P5, M6, M7
static const Degree MIXOLYDIAN_D15[]        = {0, 2, 5, 6, 8, 10, 13}; // P1, M2, M3, P4, P5, M6, m7
static const Degree AEOLIAN_D15[]           = {0, 2, 3, 6, 8, 10, 13}; // P1, M2, m3, P4, P5, m6, m7
static const Degree LOCRIAN_D15[]           = {0, 1, 3, 6, 7, 10, 13}; // P1, m2, m3, P4, d5, m6, m7

// Modes of the Diatonic Scale in 17 Tone Temperament.
//    0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
// i_P1, i_A1, i_m2, i_M2, i_A2, i_m3, i_M3, i_P4, i_A4, i_d5, i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7
static const Degree IONIAN_D17[]            = {0, 3, 6, 7, 10, 13, 16}; // P1, M2, M3, P4, P5, M6, M7
static const Degree DORIAN_D17[]            = {0, 3, 5, 7, 10, 13, 15}; // P1, M2, m3, P4, P5, M6, m7
static const Degree PHRYGIAN_D17[]          = {0, 2, 5, 7, 10, 12, 15}; // P1, m2, m3, P4, P5, m6, m7
static const Degree LYDIAN_D17[]            = {0, 3, 6, 8, 10, 12, 16}; // P1, M2, M3, A4, P5, M6, M7
static const Degree MIXOLYDIAN_D17[]        = {0, 3, 6, 7, 10, 13, 15}; // P1, M2, M3, P4, P5, M6, m7
static const Degree AEOLIAN_D17[]           = {0, 3, 5, 7, 10, 12, 15}; // P1, M2, m3, P4, P5, m6, m7
static const Degree LOCRIAN_D17[]           = {0, 2, 5, 7,  9, 12, 15}; // P1, m2, m3, P4, d5, m6, m7

// Modes of the Diatonic Scale in 19 Tone Temperament.
//    0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16    17    18
// i_P1, i_A1, i_m2, i_M2, i_A2, i_m3, i_M3, i_A3, i_P4, i_A4, i_d5, i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7, i_A7};
static const Degree IONIAN_D19[]            = {0, 3, 6, 8, 11, 14, 17}; // P1, M2, M3, P4, P5, M6, M7
static const Degree DORIAN_D19[]            = {0, 3, 5, 8, 11, 14, 16}; // P1, M2, m3, P4, P5, M6, m7
static const Degree PHRYGIAN_D19[]          = {0, 2, 5, 8, 11, 13, 16}; // P1, m2, m3, P4, P5, m6, m7
static const Degree LYDIAN_D19[]            = {0, 3, 6, 9, 11, 14, 17}; // P1, M2, M3, A4, P5, M6, M7
static const Degree MIXOLYDIAN_D19[]        = {0, 3, 6, 8, 11, 14, 16}; // P1, M2, M3, P4, P5, M6, m7
static const Degree AEOLIAN_D19[]           = {0, 3, 5, 8, 11, 13, 16}; // P1, M2, m3, P4, P5, m6, m7
static const Degree LOCRIAN_D19[]           = {0, 2, 5, 8, 10, 13, 16}; // P1, m2, m3, P4, d5, m6, m7



// Reusable 7-note scale weight presets.
// Index 0 maps to the first degree in the selected scale degree map.
// These are relative values; they do not need to sum to 1.0f.
static const char s_Uniform[] = "UNIFORM";
static const char s_Tonic_Heavy[] = "TONIC_HEAVY";
static const char s_Chord_Tone_Heavy[] = "CHORD_TONE_HEAVY";

static const float SCALE_WEIGHTS_7_UNIFORM[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const float SCALE_WEIGHTS_7_TONIC_HEAVY[] = {5.0f, 1.0f, 1.5f, 1.0f, 3.0f, 1.0f, 1.5f};
static const float SCALE_WEIGHTS_7_CHORD_TONE_HEAVY[] = {4.0f, 0.75f, 3.0f, 0.75f, 3.5f, 0.75f, 1.5f};

// Reusable 6-note scale weight presets.
static const float SCALE_WEIGHTS_6_UNIFORM[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const float SCALE_WEIGHTS_6_TONIC_HEAVY[] = {5.0f, 1.0f, 1.5f, 1.0f, 3.0f, 1.25f};
static const float SCALE_WEIGHTS_6_CHORD_TONE_HEAVY[] = {4.0f, 0.8f, 3.0f, 1.0f, 3.5f, 1.25f};

// Reusable 5-note scale weight presets.
static const float SCALE_WEIGHTS_5_UNIFORM[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
static const float SCALE_WEIGHTS_5_TONIC_HEAVY[] = {5.0f, 1.25f, 1.75f, 3.5f, 1.5f};
static const float SCALE_WEIGHTS_5_CHORD_TONE_HEAVY[] = {4.0f, 1.0f, 3.0f, 3.5f, 1.5f};

static const TemperamentTable TEMPERAMENT_TABLES[] = {
    {12, NOTE_NAMES_12, INTERVAL_NAMES_12},
    {15, NOTE_NAMES_15, INTERVAL_NAMES_15},
    {17, NOTE_NAMES_17, INTERVAL_NAMES_17},
    {19, NOTE_NAMES_19, INTERVAL_NAMES_19}
};
constexpr size_t NUM_TEMPERAMENTS = ArrayLen(TEMPERAMENT_TABLES);

static const ScaleTable SCALE_TABLES[] = {
    // 12 Tone
    {12, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D12},
    {12, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D12},
    {12, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D12},
    {12, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D12},
    {12, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D12},
    {12, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D12},
    {12, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D12},
    // 15 Tone
    {15, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D15},
    {15, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D15},
    {15, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D15},
    {15, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D15},
    {15, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D15},
    {15, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D15},
    {15, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D15},
    // 17 Tone
    {17, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D17},
    {17, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D17},
    {17, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D17},
    {17, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D17},
    {17, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D17},
    {17, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D17},
    {17, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D17},
    // 19 Tone
    {19, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D19},
    {19, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D19},
    {19, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D19},
    {19, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D19},
    {19, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D19},
    {19, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D19},
    {19, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D19}
};
constexpr size_t NUM_SCALES = ArrayLen(SCALE_TABLES);

static const WeightTable WEIGHT_TABLES[] = {
    // Heptatonic Scales
    {7, s_Uniform, SCALE_WEIGHTS_7_UNIFORM},
    {7, s_Tonic_Heavy, SCALE_WEIGHTS_7_TONIC_HEAVY},
    {7, s_Chord_Tone_Heavy, SCALE_WEIGHTS_7_CHORD_TONE_HEAVY},

    // Hexatonic Scales
    {6, s_Uniform, SCALE_WEIGHTS_6_UNIFORM},
    {6, s_Tonic_Heavy, SCALE_WEIGHTS_6_TONIC_HEAVY},
    {6, s_Chord_Tone_Heavy, SCALE_WEIGHTS_6_CHORD_TONE_HEAVY},

    // Pentatonic Scales
    {5, s_Uniform, SCALE_WEIGHTS_5_UNIFORM},
    {5, s_Tonic_Heavy, SCALE_WEIGHTS_5_TONIC_HEAVY},
    {5, s_Chord_Tone_Heavy, SCALE_WEIGHTS_5_CHORD_TONE_HEAVY}
};
constexpr size_t NUM_WEIGHTS = ArrayLen(WEIGHT_TABLES);

// clang-format on

    //
    // Validation
    //
    static_assert(ArrayLen(NOTE_NAMES_12) == 12, "NOTE_NAMES_12 array length mismatch");
    static_assert(ArrayLen(INTERVAL_NAMES_12) == 12, "INTERVAL_NAMES_12 array length mismatch");
    static_assert(ArrayLen(NOTE_NAMES_15) == 15, "NOTE_NAMES_15 array length mismatch");
    static_assert(ArrayLen(INTERVAL_NAMES_15) == 15, "INTERVAL_NAMES_15 array length mismatch");
    static_assert(ArrayLen(NOTE_NAMES_17) == 17, "NOTE_NAMES_17 array length mismatch");
    static_assert(ArrayLen(INTERVAL_NAMES_17) == 17, "INTERVAL_NAMES_17 array length mismatch");
    static_assert(ArrayLen(NOTE_NAMES_19) == 19, "NOTE_NAMES_19 array length mismatch");
    static_assert(ArrayLen(INTERVAL_NAMES_19) == 19, "INTERVAL_NAMES_19 array length mismatch");

    static_assert(ArrayLen(IONIAN_D12) == 7, "IONIAN_D12 array length mismatch");
    static_assert(ArrayLen(DORIAN_D12) == 7, "DORIAN_D12 array length mismatch");
    static_assert(ArrayLen(PHRYGIAN_D12) == 7, "PHRYGIAN_D12 array length mismatch");
    static_assert(ArrayLen(LYDIAN_D12) == 7, "LYDIAN_D12 array length mismatch");
    static_assert(ArrayLen(MIXOLYDIAN_D12) == 7, "MIXOLYDIAN_D12 array length mismatch");
    static_assert(ArrayLen(AEOLIAN_D12) == 7, "AEOLIAN_D12 array length mismatch");
    static_assert(ArrayLen(LOCRIAN_D12) == 7, "LOCRIAN_D12 array length mismatch");

    static_assert(ArrayLen(IONIAN_D15) == 7, "IONIAN_D15 array length mismatch");
    static_assert(ArrayLen(DORIAN_D15) == 7, "DORIAN_D15 array length mismatch");
    static_assert(ArrayLen(PHRYGIAN_D15) == 7, "PHRYGIAN_D15 array length mismatch");
    static_assert(ArrayLen(LYDIAN_D15) == 7, "LYDIAN_D15 array length mismatch");
    static_assert(ArrayLen(MIXOLYDIAN_D15) == 7, "MIXOLYDIAN_D15 array length mismatch");
    static_assert(ArrayLen(AEOLIAN_D15) == 7, "AEOLIAN_D15 array length mismatch");
    static_assert(ArrayLen(LOCRIAN_D15) == 7, "LOCRIAN_D15 array length mismatch");

    static_assert(ArrayLen(IONIAN_D17) == 7, "IONIAN_D17 array length mismatch");
    static_assert(ArrayLen(DORIAN_D17) == 7, "DORIAN_D17 array length mismatch");
    static_assert(ArrayLen(PHRYGIAN_D17) == 7, "PHRYGIAN_D17 array length mismatch");
    static_assert(ArrayLen(LYDIAN_D17) == 7, "LYDIAN_D17 array length mismatch");
    static_assert(ArrayLen(MIXOLYDIAN_D17) == 7, "MIXOLYDIAN_D17 array length mismatch");
    static_assert(ArrayLen(AEOLIAN_D17) == 7, "AEOLIAN_D17 array length mismatch");
    static_assert(ArrayLen(LOCRIAN_D17) == 7, "LOCRIAN_D17 array length mismatch");

    static_assert(ArrayLen(IONIAN_D19) == 7, "IONIAN_D19 array length mismatch");
    static_assert(ArrayLen(DORIAN_D19) == 7, "DORIAN_D19 array length mismatch");
    static_assert(ArrayLen(PHRYGIAN_D19) == 7, "PHRYGIAN_D19 array length mismatch");
    static_assert(ArrayLen(LYDIAN_D19) == 7, "LYDIAN_D19 array length mismatch");
    static_assert(ArrayLen(MIXOLYDIAN_D19) == 7, "MIXOLYDIAN_D19 array length mismatch");
    static_assert(ArrayLen(AEOLIAN_D19) == 7, "AEOLIAN_D19 array length mismatch");
    static_assert(ArrayLen(LOCRIAN_D19) == 7, "LOCRIAN_D19 array length mismatch");

}; // namespace Music
