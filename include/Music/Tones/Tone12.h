/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Tone12.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief 
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/Music.h>

namespace Music
{
////////////////////////////////////////////////////////////////////////////////
// 12 Tone Temperaments
//    0     1     2     3     4     5     6     7     8     9    10    11
// i_P1, i_m2, i_M2, i_m3, i_M3, i_P4, i_TT, i_P5, i_m6, i_M6, i_m7, i_M7};
////////////////////////////////////////////////////////////////////////////////
// For mapping 12 tone scales
#define P1 0
#define m2 1
#define M2 2
#define A2 3
#define d3 2
#define m3 3
#define M3 4
#define d4 4
#define P4 5
#define A4 6
#define d5 6
#define P5 7
#define A5 8
#define d6 7
#define m6 8
#define M6 9
#define d7 9
#define m7 10
#define M7 11

//
// Note & Interval Names
//
static const std::array<const char *, 12> NOTE_NAMES_12 =
    {n_C, n_Cs_Db, n_D, n_Ds_Eb, n_E, n_F, n_Fs_Gb, n_G, n_Gs_Ab, n_A, n_As_Bb, n_B};

static const std::array<const char *, 12> INTERVAL_NAMES_12 =
    {i_P1, i_m2, i_M2, i_m3, i_M3, i_P4, i_TT, i_P5, i_m6, i_M6, i_m7, i_M7};

// clang-format off


///////////////////////////////////////////////////////////////////////////////
// PENTATONIC MAPS
///////////////////////////////////////////////////////////////////////////////
static constexpr DegreeMap<PENTATONIC> MAJOR_PENTATONIC_D12       = {P1, M2, M3, P5, M6};
static constexpr DegreeMap<PENTATONIC> SUSPENDED_PENTATONIC_D12   = {P1, M2, P4, P5, m7};
static constexpr DegreeMap<PENTATONIC> MINOR_BLUES_PENTATONIC_D12 = {P1, m3, P4, m6, m7};
static constexpr DegreeMap<PENTATONIC> MAJOR_BLUES_PENTATONIC_D12 = {P1, M2, P4, P5, M6};
static constexpr DegreeMap<PENTATONIC> MINOR_PENTATONIC_D12       = {P1, m3, P4, P5, m7};

///////////////////////////////////////////////////////////////////////////////
// HEXATONIC MAPS
///////////////////////////////////////////////////////////////////////////////
static constexpr DegreeMap<HEXATONIC> WHOLE_TONE_HEXATONIC_D12[] = {P1, M2, M3, A4, A5, M6};
static constexpr DegreeMap<HEXATONIC> BLUES_HEXATONIC_D12[]      = {P1, m3, P4, d5, P5, m7};
static constexpr DegreeMap<HEXATONIC> AUGMENTED_HEXATONIC_D12[]  = {P1, m3, M3, A4, A5, M6};

///////////////////////////////////////////////////////////////////////////////
// HEPATONIC MAPS
///////////////////////////////////////////////////////////////////////////////
// Modes of the Diatonic Scale in 12 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> IONIAN_D12                   = {P1, M2, M3, P4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> DORIAN_D12[]                 = {P1, M2, m3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> PHRYGIAN_D12[]               = {P1, m2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_D12[]                 = {P1, M2, M3, A4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> MIXOLYDIAN_D12[]             = {P1, M2, M3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> AEOLIAN_D12[]                = {P1, M2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_D12[]                = {P1, m2, m3, P4, d5, m6, m7};
// Modes of the Harmonic Minor Scale in 12 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> HARMONIC_MINOR_D12[]         = {P1, M2, m3, P4, P5, m6, M7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_s6_D12[]             = {P1, m2, m3, P4, d5, M6, m7};
static constexpr DegreeMap<HEPATONIC> IONIAN_s5_D12[]              = {P1, M2, M3, P4, m6, M6, M7};
static constexpr DegreeMap<HEPATONIC> DORIAN_s4_D12[]              = {P1, M2, m3, A4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> PHRYGIAN_DOMINANT_D12[]      = {P1, m2, M3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_s2_DEGREES12[]        = {P1, m3, M3, A4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> ULTRALOCRIAN_D12[]           = {P1, m2, m3, M3, d5, m6, M6};
// Modes of the Melodic Minor Scale in 12 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> MELODIC_MINOR_D12[]          = {P1, M2, m3, P4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> DORIAN_b2_D12[]              = {P1, m2, m3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_AUG_D12             = {P1, M2, M3, A4, A5, M6, M7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_DOMINANT_D12        = {P1, M2, M3, A4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> MIXOLYDIAN_b6_D12          = {P1, M2, M3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_s2_D12             = {P1, M2, m3, P4, d5, m6, m7};
static constexpr DegreeMap<HEPATONIC> SUPER_LOCRIAN_D12          = {P1, m2, m3, d4, d5, m6, m7};     
// Modes of the Harmonic DegreeMap<HEPATONIC> Scale in 12 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> HARMONIC_MAJOR_D12         = {P1, M2, M3, P4, P5, m6, M7};
static constexpr DegreeMap<HEPATONIC> DORIAN_b5_D12              = {P1, M2, m3, P4, d5, m6, m7};
static constexpr DegreeMap<HEPATONIC> PHRYGIAN_b4_D12            = {P1, m2, m3, d4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_b3_D12              = {P1, M2, m3, A4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> MIXOLYDIAN_b2_D12          = {P1, m2, M3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_AUG_s2_D12          = {P1, A2, M3, A4, A5, M6, M7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_b7_D12             = {P1, m2, m3, P4, d5, m6, d7};
// Modes of the Double Harmonic Scale in 12 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> DOUBLE_HARMONIC_D12        = {P1, m2, M3, P4, P5, m6, M7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_b6_b7_D12           = {P1, M2, M3, A4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> ULTRAPHRYGIAN_D12          = {P1, m2, m3, d4, P5, m6, d7};
static constexpr DegreeMap<HEPATONIC> HUNGARIAN_MINOR_D12        = {P1, M2, m3, A4, P5, m6, M7};
static constexpr DegreeMap<HEPATONIC> ORIENTAL_D12               = {P1, m2, M3, P4, A5, M6, M7};
static constexpr DegreeMap<HEPATONIC> IONIAN_s2_s5_D12           = {P1, A2, M3, P4, A5, M6, M7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_bb3_bb7_D12        = {P1, m2, d3, P4, d5, m6, d7};
// clang-format on

#undef P1
#undef m2
#undef M2
#undef A2
#undef d3
#undef m3
#undef M3
#undef d4
#undef P4
#undef A4
#undef d5
#undef P5
#undef A5
#undef d6
#undef m6
#undef M6
#undef d7
#undef m7
#undef M7

} // namespace Music
