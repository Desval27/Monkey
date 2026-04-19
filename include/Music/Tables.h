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

#include <Music/Music.h>

#include <Music/Tone12.h>
#include <Music/Tone15.h>
#include <Music/Tone17.h>
#include <Music/Tone19.h>

namespace Music
{
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

    // clang-format on                        1234567890 
    static const char *s_IONIAN            = "IONIAN";
    static const char *s_DORIAN            = "DORIAN";
    static const char *s_PHRYGIAN          = "PHRYGIAN";
    static const char *s_LYDIAN            = "LYDIAN";
    static const char *s_MIXOLYDIAN        = "MIXOLYDIAN";
    static const char *s_AEOLIAN           = "AEOLIAN";
    static const char *s_LOCRIAN           = "LOCRIAN";

    static const char *s_HARMONIC_MINOR    = "HARMONIC MINOR";
    static const char *s_LOCRIAN_s6        = "LOCRIAN #6";
    static const char *s_IONIAN_s5         = "IONIAN #5";
    static const char *s_DORIAN_s4         = "DORIAN #4";
    static const char *s_PHYRGIAN_DOMINANT = "PHYR DOMINANT";
    static const char *s_LYDIAN_s2         = "LYDIAN #2";
    static const char *s_ULTRALOCRIAN      = "ULTRALOCRIAN";

    static const char *s_MELODIC_MINOR     = "MELODIC MINOR";
    static const char *s_DORIAN_b2         = "DORIAN b2";
    static const char *s_LYDIAN_AUG        = "LYDIAN AUGMENTED";
    static const char *s_LYDIAN_DOMINANT   = "LYDIAN DOMINANT";
    static const char *s_MIXOLYDIAN_b6     = "MIXOLYD b6";
    static const char *s_LOCRIAN_s2        = "LOCRIAN #2";
    static const char *s_SUPER_LOCRIAN     = "SUPER LOCRIAN";

    static const char *s_HARMONIC_MAJOR    = "HARMONIC MAJOR";
    static const char *s_DORIAN_b5         = "DORIAN b5";
    static const char *s_PHRYGIAN_b4       = "PHRYGIAN b4";
    static const char *s_LYDIAN_b3         = "LYDIAN b3";
    static const char *s_MIXOLYDIAN_b2     = "MIXOLYDIAN b2";
    static const char *s_LYDIAN_AUG_s2     = "LYDIAN AUG #2";
    static const char *s_LOCRIAN_b7        = "LOCRIAN b7";

    static const char *s_DOUBLE_HARMONIC   = "DBL HARMONIC MAJOR";
    static const char *s_LYDIAN_b6_b7      = "LYDIAN b6b7";
    static const char *s_ULTRAPHRYGIAN     = "ULTRAPHRYGIAN";
    static const char *s_HUNGARIAN_MINOR   = "HUNGARIAN MINOR";
    static const char *s_ORIENTAL          = "ORIENTAL";
    static const char *s_IONIAN_s2_s5      = "IONIAN #2#5";
    static const char *s_LOCRIAN_bb3_bb7   = "LOCRIAN bb3bb7";

    // clang-format on

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
        n_C, n_Cs_Db, n_D, n_Ds, n_Eb, n_E, n_F, n_Fs_Gb, n_G, n_Gs, n_Ab, n_A, n_As, n_Bb, n_B};

    static const char *const INTERVAL_NAMES_15[] = {
        // 0     1     2     3     4     5     6     7     8     9    10    11    12    13    14
        i_P1, i_m2, i_M2, i_A2, i_m3, i_M3, i_P4, i_TT, i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7};

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
        {19, NOTE_NAMES_19, INTERVAL_NAMES_19}};
    constexpr size_t NUM_TEMPERAMENTS = ArrayLen(TEMPERAMENT_TABLES);

    // For temporary rigging
    constexpr int D12StartIndex = 0;
    constexpr int D12Count      = 5*7;
    static const ScaleTable SCALE_TABLES[] = {
        // 12 Tone
        {12, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D12, ArrayLen(IONIAN_D12)},
        {12, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D12, ArrayLen(DORIAN_D12)},
        {12, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D12, ArrayLen(PHRYGIAN_D12)},
        {12, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D12, ArrayLen(LYDIAN_D12)},
        {12, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D12, ArrayLen(MIXOLYDIAN_D12)},
        {12, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D12, ArrayLen(AEOLIAN_D12)},
        {12, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D12, ArrayLen(LOCRIAN_D12)},

        {12, 7, s_HARMONIC_MINOR, HarmonicMode::Minor, HARMONIC_MINOR_D12, ArrayLen(HARMONIC_MINOR_D12)},
        {12, 7, s_LOCRIAN_s6, HarmonicMode::Minor, LOCRIAN_s6_D12, ArrayLen(LOCRIAN_s6_D12)},
        {12, 7, s_IONIAN_s5, HarmonicMode::Major, IONIAN_s5_D12, ArrayLen(IONIAN_s5_D12)},
        {12, 7, s_DORIAN_s4, HarmonicMode::Minor, DORIAN_s4_D12, ArrayLen(DORIAN_s4_D12)},
        {12, 7, s_PHYRGIAN_DOMINANT, HarmonicMode::Major, PHRYGIAN_DOMINANT_D12, ArrayLen(PHRYGIAN_DOMINANT_D12)},
        {12, 7, s_LYDIAN_s2, HarmonicMode::Major, LYDIAN_s2_DEGREES12, ArrayLen(LYDIAN_s2_DEGREES12)},
        {12, 7, s_ULTRALOCRIAN, HarmonicMode::Minor, ULTRALOCRIAN_D12, ArrayLen(ULTRALOCRIAN_D12)},

        {12, 7, s_MELODIC_MINOR, HarmonicMode::Minor, MELODIC_MINOR_D12, ArrayLen(MELODIC_MINOR_D12)},
        {12, 7, s_DORIAN_b2, HarmonicMode::Minor, DORIAN_b2_D12, ArrayLen(DORIAN_b2_D12)},
        {12, 7, s_LYDIAN_AUG, HarmonicMode::Major, LYDIAN_AUG_D12, ArrayLen(LYDIAN_AUG_D12)},
        {12, 7, s_LYDIAN_DOMINANT, HarmonicMode::Major, LYDIAN_DOMINANT_D12, ArrayLen(LYDIAN_DOMINANT_D12)},
        {12, 7, s_MIXOLYDIAN_b6, HarmonicMode::Major, MIXOLYDIAN_b6_D12, ArrayLen(MIXOLYDIAN_b6_D12)},
        {12, 7, s_LOCRIAN_s2, HarmonicMode::Minor, LOCRIAN_s2_D12, ArrayLen(LOCRIAN_s2_D12)},
        {12, 7, s_SUPER_LOCRIAN, HarmonicMode::Minor, SUPER_LOCRIAN_D12, ArrayLen(SUPER_LOCRIAN_D12)},

        {12, 7, s_HARMONIC_MAJOR, HarmonicMode::Major, HARMONIC_MAJOR_D12, ArrayLen(HARMONIC_MAJOR_D12)},
        {12, 7, s_DORIAN_b5, HarmonicMode::Minor, DORIAN_b5_D12, ArrayLen(DORIAN_b5_D12)},
        {12, 7, s_PHRYGIAN_b4, HarmonicMode::Minor, PHRYGIAN_b4_D12, ArrayLen(PHRYGIAN_b4_D12)},
        {12, 7, s_LYDIAN_b3, HarmonicMode::Minor, LYDIAN_b3_D12, ArrayLen(LYDIAN_b3_D12)},
        {12, 7, s_MIXOLYDIAN_b6, HarmonicMode::Major, MIXOLYDIAN_b2_D12, ArrayLen(MIXOLYDIAN_b2_D12)},
        {12, 7, s_LYDIAN_AUG_s2, HarmonicMode::Major, LYDIAN_AUG_s2_D12, ArrayLen(LYDIAN_AUG_s2_D12)},
        {12, 7, s_LOCRIAN_b7, HarmonicMode::Minor, LOCRIAN_b7_D12, ArrayLen(LOCRIAN_b7_D12)},

        {12, 7, s_DOUBLE_HARMONIC, HarmonicMode::Major, DOUBLE_HARMONIC_D12, ArrayLen(DOUBLE_HARMONIC_D12)},
        {12, 7, s_LYDIAN_b6_b7, HarmonicMode::Major, LYDIAN_b6_b7_D12, ArrayLen(LYDIAN_b6_b7_D12)},
        {12, 7, s_ULTRAPHRYGIAN, HarmonicMode::Minor, ULTRAPHRYGIAN_D12, ArrayLen(ULTRAPHRYGIAN_D12)},
        {12, 7, s_HUNGARIAN_MINOR, HarmonicMode::Minor, HUNGARIAN_MINOR_D12, ArrayLen(HUNGARIAN_MINOR_D12)},
        {12, 7, s_ORIENTAL, HarmonicMode::Major, ORIENTAL_D12, ArrayLen(ORIENTAL_D12)},
        {12, 7, s_IONIAN_s2_s5, HarmonicMode::Major, IONIAN_s2_s5_D12, ArrayLen(IONIAN_s2_s5_D12)},
        {12, 7, s_LOCRIAN_bb3_bb7, HarmonicMode::Minor, LOCRIAN_bb3_bb7_D12, ArrayLen(LOCRIAN_bb3_bb7_D12)},

        // 15 Tone
        {15, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D15, ArrayLen(IONIAN_D15)},
        {15, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D15, ArrayLen(DORIAN_D15)},
        {15, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D15, ArrayLen(PHRYGIAN_D15)},
        {15, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D15, ArrayLen(LYDIAN_D15)},
        {15, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D15, ArrayLen(MIXOLYDIAN_D15)},
        {15, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D15, ArrayLen(AEOLIAN_D15)},
        {15, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D15, ArrayLen(LOCRIAN_D15)},
        // 17 Tone
        {17, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D17, ArrayLen(IONIAN_D17)},
        {17, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D17, ArrayLen(DORIAN_D17)},
        {17, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D17, ArrayLen(PHRYGIAN_D17)},
        {17, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D17, ArrayLen(LYDIAN_D17)},
        {17, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D17, ArrayLen(MIXOLYDIAN_D17)},
        {17, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D17, ArrayLen(AEOLIAN_D17)},
        {17, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D17, ArrayLen(LOCRIAN_D17)},
        // 19 Tone
        {19, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D19, ArrayLen(IONIAN_D19)},
        {19, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D19, ArrayLen(DORIAN_D19)},
        {19, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D19, ArrayLen(PHRYGIAN_D19)},
        {19, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D19, ArrayLen(LYDIAN_D19)},
        {19, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D19, ArrayLen(MIXOLYDIAN_D19)},
        {19, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D19, ArrayLen(AEOLIAN_D19)},
        {19, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D19, ArrayLen(LOCRIAN_D19)}};
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
        {5, s_Chord_Tone_Heavy, SCALE_WEIGHTS_5_CHORD_TONE_HEAVY}};
    constexpr size_t NUM_WEIGHTS = ArrayLen(WEIGHT_TABLES);

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

}; // namespace Music
