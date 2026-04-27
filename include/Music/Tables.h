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

#include <array>

#include <Music/Labels/NoteValueLabels.h>
#include <Music/Labels/NoteNameLabels.h>
#include <Music/Labels/IntervalNameLabels.h>
#include <Music/Labels/ScaleNameLabels.h>
#include <Music/Labels/OtherLabels.h>

#include <Music/Tones/Tone12.h>
#include <Music/Tones/Tone15.h>
#include <Music/Tones/Tone17.h>
#include <Music/Tones/Tone19.h>
#include <Music/Tones/Tone22.h>

namespace Music
{
    // clang-format off

    // Reusable 7-note scale weight presets.

    // Index 0 maps to the first degree in the selected scale degree map.
    // These are relative values; they do not need to sum to 1.0f.
    static const WeightMap<PENTATONIC> SCALE_WEIGHTS_5_UNIFORM = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    static const WeightMap<PENTATONIC> SCALE_WEIGHTS_5_TONIC_HEAVY = {5.0f, 1.25f, 1.75f, 3.5f, 1.5f};
    static const WeightMap<PENTATONIC> SCALE_WEIGHTS_5_CHORD_TONE_HEAVY = {4.0f, 1.0f, 3.0f, 3.5f, 1.5f};

    static const WeightMap<HEXATONIC> SCALE_WEIGHTS_6_UNIFORM = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    static const WeightMap<HEXATONIC> SCALE_WEIGHTS_6_TONIC_HEAVY = {5.0f, 1.0f, 1.5f, 1.0f, 3.0f, 1.25f};
    static const WeightMap<HEXATONIC> SCALE_WEIGHTS_6_CHORD_TONE_HEAVY = {4.0f, 0.8f, 3.0f, 1.0f, 3.5f, 1.25f};

    static const WeightMap<HEPATONIC> SCALE_WEIGHTS_7_UNIFORM = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    static const WeightMap<HEPATONIC> SCALE_WEIGHTS_7_TONIC_HEAVY = {5.0f, 1.0f, 1.5f, 1.0f, 3.0f, 1.0f, 1.5f};
    static const WeightMap<HEPATONIC> SCALE_WEIGHTS_7_CHORD_TONE_HEAVY = {4.0f, 0.75f, 3.0f, 0.75f, 3.5f, 0.75f, 1.5f};

    // Reusable 5-note scale weight presets.

    static const std::array<EqualTemperamentTable, 5> EQUAL_TEMPERAMENT_TABLE = {
        EqualTemperamentTable{NOTE_NAMES_12.size(), s_EDO12, NOTE_NAMES_12.data(), INTERVAL_NAMES_12.data()},
        EqualTemperamentTable{NOTE_NAMES_15.size(), s_EDO15, NOTE_NAMES_15.data(), INTERVAL_NAMES_15.data()},
        EqualTemperamentTable{NOTE_NAMES_17.size(), s_EDO17, NOTE_NAMES_17.data(), INTERVAL_NAMES_17.data()},
        EqualTemperamentTable{NOTE_NAMES_19.size(), s_EDO19, NOTE_NAMES_19.data(), INTERVAL_NAMES_19.data()},
        EqualTemperamentTable{NOTE_NAMES_22.size(), s_EDO22, NOTE_NAMES_22.data(), INTERVAL_NAMES_22.data()},
    };

    static const WeightTable<PENTATONIC> PENTATONIC_WEIGHT_TABLES[] = {
        {s_Uniform, SCALE_WEIGHTS_5_UNIFORM},
        {s_Tonic_Heavy, SCALE_WEIGHTS_5_TONIC_HEAVY},
        {s_Chord_Tone_Heavy, SCALE_WEIGHTS_5_CHORD_TONE_HEAVY},
    };

    static const WeightTable<HEXATONIC> HEXATONIC_WEIGHT_TABLES[] = {
        {s_Uniform, SCALE_WEIGHTS_6_UNIFORM},
        {s_Tonic_Heavy, SCALE_WEIGHTS_6_TONIC_HEAVY},
        {s_Chord_Tone_Heavy, SCALE_WEIGHTS_6_CHORD_TONE_HEAVY},
    };
    
    static const WeightTable<HEPATONIC> HEPATONIC_WEIGHT_TABLES[] = {
        {s_Uniform, SCALE_WEIGHTS_7_UNIFORM},
        {s_Tonic_Heavy, SCALE_WEIGHTS_7_TONIC_HEAVY},
        {s_Chord_Tone_Heavy, SCALE_WEIGHTS_7_CHORD_TONE_HEAVY},
    };


    // // For temporary rigging
    // constexpr int D12StartIndex = 0;
    // constexpr int D12Count = 5 * 7;
    // static const ScaleTable SCALE_TABLES[] = {
    //     // 12 Tone
    //     {12, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D12, ArrayLen(IONIAN_D12)},
    //     {12, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D12, ArrayLen(DORIAN_D12)},
    //     {12, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D12, ArrayLen(PHRYGIAN_D12)},
    //     {12, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D12, ArrayLen(LYDIAN_D12)},
    //     {12, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D12, ArrayLen(MIXOLYDIAN_D12)},
    //     {12, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D12, ArrayLen(AEOLIAN_D12)},
    //     {12, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D12, ArrayLen(LOCRIAN_D12)},

    //     {12, 7, s_HARMONIC_MINOR, HarmonicMode::Minor, HARMONIC_MINOR_D12, ArrayLen(HARMONIC_MINOR_D12)},
    //     {12, 7, s_LOCRIAN_s6, HarmonicMode::Minor, LOCRIAN_s6_D12, ArrayLen(LOCRIAN_s6_D12)},
    //     {12, 7, s_IONIAN_s5, HarmonicMode::Major, IONIAN_s5_D12, ArrayLen(IONIAN_s5_D12)},
    //     {12, 7, s_DORIAN_s4, HarmonicMode::Minor, DORIAN_s4_D12, ArrayLen(DORIAN_s4_D12)},
    //     {12, 7, s_PHYRGIAN_DOMINANT, HarmonicMode::Major, PHRYGIAN_DOMINANT_D12, ArrayLen(PHRYGIAN_DOMINANT_D12)},
    //     {12, 7, s_LYDIAN_s2, HarmonicMode::Major, LYDIAN_s2_DEGREES12, ArrayLen(LYDIAN_s2_DEGREES12)},
    //     {12, 7, s_ULTRALOCRIAN, HarmonicMode::Minor, ULTRALOCRIAN_D12, ArrayLen(ULTRALOCRIAN_D12)},

    //     {12, 7, s_MELODIC_MINOR, HarmonicMode::Minor, MELODIC_MINOR_D12, ArrayLen(MELODIC_MINOR_D12)},
    //     {12, 7, s_DORIAN_b2, HarmonicMode::Minor, DORIAN_b2_D12, ArrayLen(DORIAN_b2_D12)},
    //     {12, 7, s_LYDIAN_AUG, HarmonicMode::Major, LYDIAN_AUG_D12, ArrayLen(LYDIAN_AUG_D12)},
    //     {12, 7, s_LYDIAN_DOMINANT, HarmonicMode::Major, LYDIAN_DOMINANT_D12, ArrayLen(LYDIAN_DOMINANT_D12)},
    //     {12, 7, s_MIXOLYDIAN_b6, HarmonicMode::Major, MIXOLYDIAN_b6_D12, ArrayLen(MIXOLYDIAN_b6_D12)},
    //     {12, 7, s_LOCRIAN_s2, HarmonicMode::Minor, LOCRIAN_s2_D12, ArrayLen(LOCRIAN_s2_D12)},
    //     {12, 7, s_SUPER_LOCRIAN, HarmonicMode::Minor, SUPER_LOCRIAN_D12, ArrayLen(SUPER_LOCRIAN_D12)},

    //     {12, 7, s_HARMONIC_MAJOR, HarmonicMode::Major, HARMONIC_MAJOR_D12, ArrayLen(HARMONIC_MAJOR_D12)},
    //     {12, 7, s_DORIAN_b5, HarmonicMode::Minor, DORIAN_b5_D12, ArrayLen(DORIAN_b5_D12)},
    //     {12, 7, s_PHRYGIAN_b4, HarmonicMode::Minor, PHRYGIAN_b4_D12, ArrayLen(PHRYGIAN_b4_D12)},
    //     {12, 7, s_LYDIAN_b3, HarmonicMode::Minor, LYDIAN_b3_D12, ArrayLen(LYDIAN_b3_D12)},
    //     {12, 7, s_MIXOLYDIAN_b6, HarmonicMode::Major, MIXOLYDIAN_b2_D12, ArrayLen(MIXOLYDIAN_b2_D12)},
    //     {12, 7, s_LYDIAN_AUG_s2, HarmonicMode::Major, LYDIAN_AUG_s2_D12, ArrayLen(LYDIAN_AUG_s2_D12)},
    //     {12, 7, s_LOCRIAN_b7, HarmonicMode::Minor, LOCRIAN_b7_D12, ArrayLen(LOCRIAN_b7_D12)},

    //     {12, 7, s_DOUBLE_HARMONIC, HarmonicMode::Major, DOUBLE_HARMONIC_D12, ArrayLen(DOUBLE_HARMONIC_D12)},
    //     {12, 7, s_LYDIAN_b6_b7, HarmonicMode::Major, LYDIAN_b6_b7_D12, ArrayLen(LYDIAN_b6_b7_D12)},
    //     {12, 7, s_ULTRAPHRYGIAN, HarmonicMode::Minor, ULTRAPHRYGIAN_D12, ArrayLen(ULTRAPHRYGIAN_D12)},
    //     {12, 7, s_HUNGARIAN_MINOR, HarmonicMode::Minor, HUNGARIAN_MINOR_D12, ArrayLen(HUNGARIAN_MINOR_D12)},
    //     {12, 7, s_ORIENTAL, HarmonicMode::Major, ORIENTAL_D12, ArrayLen(ORIENTAL_D12)},
    //     {12, 7, s_IONIAN_s2_s5, HarmonicMode::Major, IONIAN_s2_s5_D12, ArrayLen(IONIAN_s2_s5_D12)},
    //     {12, 7, s_LOCRIAN_bb3_bb7, HarmonicMode::Minor, LOCRIAN_bb3_bb7_D12, ArrayLen(LOCRIAN_bb3_bb7_D12)},

    //     // 15 Tone
    //     {15, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D15, ArrayLen(IONIAN_D15)},
    //     {15, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D15, ArrayLen(DORIAN_D15)},
    //     {15, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D15, ArrayLen(PHRYGIAN_D15)},
    //     {15, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D15, ArrayLen(LYDIAN_D15)},
    //     {15, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D15, ArrayLen(MIXOLYDIAN_D15)},
    //     {15, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D15, ArrayLen(AEOLIAN_D15)},
    //     {15, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D15, ArrayLen(LOCRIAN_D15)},
    //     // 17 Tone
    //     {17, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D17, ArrayLen(IONIAN_D17)},
    //     {17, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D17, ArrayLen(DORIAN_D17)},
    //     {17, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D17, ArrayLen(PHRYGIAN_D17)},
    //     {17, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D17, ArrayLen(LYDIAN_D17)},
    //     {17, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D17, ArrayLen(MIXOLYDIAN_D17)},
    //     {17, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D17, ArrayLen(AEOLIAN_D17)},
    //     {17, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D17, ArrayLen(LOCRIAN_D17)},
    //     // 19 Tone
    //     {19, 7, s_IONIAN, HarmonicMode::Major, IONIAN_D19, ArrayLen(IONIAN_D19)},
    //     {19, 7, s_DORIAN, HarmonicMode::Minor, DORIAN_D19, ArrayLen(DORIAN_D19)},
    //     {19, 7, s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D19, ArrayLen(PHRYGIAN_D19)},
    //     {19, 7, s_LYDIAN, HarmonicMode::Major, LYDIAN_D19, ArrayLen(LYDIAN_D19)},
    //     {19, 7, s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D19, ArrayLen(MIXOLYDIAN_D19)},
    //     {19, 7, s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D19, ArrayLen(AEOLIAN_D19)},
    //     {19, 7, s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D19, ArrayLen(LOCRIAN_D19)}};
    // constexpr size_t NUM_SCALES = ArrayLen(SCALE_TABLES);


    //     // Hexatonic Scales
    //     {6, s_Uniform, SCALE_WEIGHTS_6_UNIFORM},
    //     {6, s_Tonic_Heavy, SCALE_WEIGHTS_6_TONIC_HEAVY},
    //     {6, s_Chord_Tone_Heavy, SCALE_WEIGHTS_6_CHORD_TONE_HEAVY},

    //     // Pentatonic Scales
    //     {5, s_Uniform, SCALE_WEIGHTS_5_UNIFORM},
    //     {5, s_Tonic_Heavy, SCALE_WEIGHTS_5_TONIC_HEAVY},
    //     {5, s_Chord_Tone_Heavy, SCALE_WEIGHTS_5_CHORD_TONE_HEAVY}};
    // constexpr size_t NUM_WEIGHTS = ArrayLen(WEIGHT_TABLES);

    // clang-format on

}; // namespace Music
