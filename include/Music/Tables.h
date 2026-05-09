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

#include <Music/Labels/IntervalNameLabels.h>
#include <Music/Labels/NoteNameLabels.h>
#include <Music/Labels/NoteValueLabels.h>
#include <Music/Labels/OtherLabels.h>
#include <Music/Labels/ScaleNameLabels.h>
#include <Music/MusicTypes.h>
#include <Music/Tones/Tone12.h>
#include <Music/Tones/Tone15.h>
#include <Music/Tones/Tone17.h>
#include <Music/Tones/Tone19.h>
#include <Music/Tones/Tone22.h>

#include <array>

namespace Music {
// clang-format off

    // Reusable 7-note scale weight presets.

    // Index 0 maps to the first degree in the selected scale degree map.
    // These are relative values; they do not need to sum to 1.0f.
    static const WeightMap<PENTATONIC> SCALE_WEIGHTS_5_UNIFORM = {1.0F, 1.0F, 1.0F, 1.0F, 1.0F};
    static const WeightMap<PENTATONIC> SCALE_WEIGHTS_5_TONIC_HEAVY = {5.0F, 1.25F, 1.75F, 3.5F, 1.5F};
    static const WeightMap<PENTATONIC> SCALE_WEIGHTS_5_CHORD_TONE_HEAVY = {4.0F, 1.0F, 3.0F, 3.5F, 1.5F};

    static const WeightMap<HEXATONIC> SCALE_WEIGHTS_6_UNIFORM = {1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F};
    static const WeightMap<HEXATONIC> SCALE_WEIGHTS_6_TONIC_HEAVY = {5.0F, 1.0F, 1.5F, 1.0F, 3.0F, 1.25F};
    static const WeightMap<HEXATONIC> SCALE_WEIGHTS_6_CHORD_TONE_HEAVY = {4.0F, 0.8F, 3.0F, 1.0F, 3.5F, 1.25F};

    static const WeightMap<HEPATONIC> SCALE_WEIGHTS_7_UNIFORM = {1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F, 1.0F};
    static const WeightMap<HEPATONIC> SCALE_WEIGHTS_7_TONIC_HEAVY = {5.0F, 1.0F, 1.5F, 1.0F, 3.0F, 1.0F, 1.5F};
    static const WeightMap<HEPATONIC> SCALE_WEIGHTS_7_CHORD_TONE_HEAVY = {4.0F, 0.75F, 3.0F, 0.75F, 3.5F, 0.75F, 1.5F};

    static const std::array<EqualTemperamentTable, 5> EQUAL_TEMPERAMENT_TABLE = {
        EqualTemperamentTable{12, {"EDO-12"}, NOTE_NAMES_12.data(), INTERVAL_NAMES_12.data()},
        EqualTemperamentTable{15, {"EDO-15"}, NOTE_NAMES_15.data(), INTERVAL_NAMES_15.data()},
        EqualTemperamentTable{17, {"EDO-17"}, NOTE_NAMES_17.data(), INTERVAL_NAMES_17.data()},
        EqualTemperamentTable{19, {"EDO-19"}, NOTE_NAMES_19.data(), INTERVAL_NAMES_19.data()},
        EqualTemperamentTable{22, {"EDO-22"}, NOTE_NAMES_22.data(), INTERVAL_NAMES_22.data()},
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

    // 12 Tone Pentatonic Scales
    static const ScaleTable<12, PENTATONIC> PENTATONIC_D12_SCALES[] = {
        {s_MAJOR_PENTATONIC, HarmonicMode::Major, MAJOR_PENTATONIC_D12},
        {s_SUSPENDED_PENTATONIC, HarmonicMode::Minor, SUSPENDED_PENTATONIC_D12},
        {s_BLUES_MINOR_PENTATONIC, HarmonicMode::Minor, BLUES_MINOR_PENTATONIC_D12},
        {s_BLUES_MAJOR_PENTATONIC, HarmonicMode::Major, BLUES_MAJOR_PENTATONIC_D12},
        {s_MINOR_PENTATONIC, HarmonicMode::Minor, MINOR_PENTATONIC_D12},
    };


    // 12 Tone Hepatonic Scales
    static const ScaleTable<12, HEPATONIC> HEPATONIC_D12_SCALES[] = {
        {s_IONIAN, HarmonicMode::Major, IONIAN_D12},
        {s_DORIAN, HarmonicMode::Minor, DORIAN_D12},
        {s_PHRYGIAN, HarmonicMode::Minor, PHRYGIAN_D12},
        {s_LYDIAN, HarmonicMode::Major, LYDIAN_D12},
        {s_MIXOLYDIAN, HarmonicMode::Major, MIXOLYDIAN_D12},
        {s_AEOLIAN, HarmonicMode::Minor, AEOLIAN_D12},
        {s_LOCRIAN, HarmonicMode::Minor, LOCRIAN_D12},

        {s_HARMONIC_MINOR, HarmonicMode::Minor, HARMONIC_MINOR_D12},
        {s_LOCRIAN_s6, HarmonicMode::Minor, LOCRIAN_s6_D12},
        {s_IONIAN_s5, HarmonicMode::Major, IONIAN_s5_D12},
        {s_DORIAN_s4, HarmonicMode::Minor, DORIAN_s4_D12},
        {s_PHYRGIAN_DOMINANT, HarmonicMode::Major, PHRYGIAN_DOMINANT_D12},
        {s_LYDIAN_s2, HarmonicMode::Major, LYDIAN_s2_DEGREES12},
        {s_ULTRALOCRIAN, HarmonicMode::Minor, ULTRALOCRIAN_D12},

        {s_MELODIC_MINOR, HarmonicMode::Minor, MELODIC_MINOR_D12},
        {s_DORIAN_b2, HarmonicMode::Minor, DORIAN_b2_D12},
        {s_LYDIAN_AUG, HarmonicMode::Major, LYDIAN_AUG_D12},
        {s_LYDIAN_DOMINANT, HarmonicMode::Major, LYDIAN_DOMINANT_D12},
        {s_MIXOLYDIAN_b6, HarmonicMode::Major, MIXOLYDIAN_b6_D12},
        {s_LOCRIAN_s2, HarmonicMode::Minor, LOCRIAN_s2_D12},
        {s_SUPER_LOCRIAN, HarmonicMode::Minor, SUPER_LOCRIAN_D12},

        {s_HARMONIC_MAJOR, HarmonicMode::Major, HARMONIC_MAJOR_D12},
        {s_DORIAN_b5, HarmonicMode::Minor, DORIAN_b5_D12},
        {s_PHRYGIAN_b4, HarmonicMode::Minor, PHRYGIAN_b4_D12},
        {s_LYDIAN_b3, HarmonicMode::Minor, LYDIAN_b3_D12},
        {s_MIXOLYDIAN_b6, HarmonicMode::Major, MIXOLYDIAN_b2_D12},
        {s_LYDIAN_AUG_s2, HarmonicMode::Major, LYDIAN_AUG_s2_D12},
        {s_LOCRIAN_b7, HarmonicMode::Minor, LOCRIAN_b7_D12},

        {s_DOUBLE_HARMONIC, HarmonicMode::Major, DOUBLE_HARMONIC_D12},
        {s_LYDIAN_b6_b7, HarmonicMode::Major, LYDIAN_b6_b7_D12},
        {s_ULTRAPHRYGIAN, HarmonicMode::Minor, ULTRAPHRYGIAN_D12},
        {s_HUNGARIAN_MINOR, HarmonicMode::Minor, HUNGARIAN_MINOR_D12},
        {s_ORIENTAL, HarmonicMode::Major, ORIENTAL_D12},
        {s_IONIAN_s2_s5, HarmonicMode::Major, IONIAN_s2_s5_D12},
        {s_LOCRIAN_bb3_bb7, HarmonicMode::Minor, LOCRIAN_bb3_bb7_D12}
    };
    //constexpr std::size_t NUM_SCALES = ArrayLen(SCALE_TABLES);


    //     // Hexatonic Scales
    //     {6, s_Uniform, SCALE_WEIGHTS_6_UNIFORM},
    //     {6, s_Tonic_Heavy, SCALE_WEIGHTS_6_TONIC_HEAVY},
    //     {6, s_Chord_Tone_Heavy, SCALE_WEIGHTS_6_CHORD_TONE_HEAVY},

    //     // Pentatonic Scales
    //     {5, s_Uniform, SCALE_WEIGHTS_5_UNIFORM},
    //     {5, s_Tonic_Heavy, SCALE_WEIGHTS_5_TONIC_HEAVY},
    //     {5, s_Chord_Tone_Heavy, SCALE_WEIGHTS_5_CHORD_TONE_HEAVY}};
    // constexpr std::size_t NUM_WEIGHTS = ArrayLen(WEIGHT_TABLES);

// clang-format on

}; // namespace Music
