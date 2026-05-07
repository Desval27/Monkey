/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Tone15.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <array>

#include <Music/Labels/IntervalNameLabels.h>
#include <Music/Labels/NoteNameLabels.h>
#include <Music/MusicTypes.h>

namespace Music {
//////////////////////////////////////////////////////////////////////////////////////////////////
// 15 Tone Temperaments
//    0     1     2     3     4     5     6     7     8     9    10    11    12
//    13    14
// i_P1, i_m2, i_M2, i_A2, i_m3, i_M3, i_P4, i_TT, i_P5, i_A5, i_m6, i_M6, i_A6,
// i_m7, i_M7
//////////////////////////////////////////////////////////////////////////////////////////////////

// For mapping 15 tone scales
#define P1 0
#define m2 1
#define M2 2
#define A2 3
#define d3 3
#define m3 4
#define M3 5
#define d4 5
#define P4 6
#define A4 7
#define d5 7
#define P5 8
#define A5 9
#define d6 9
#define m6 10
#define M6 11
#define A6 12
#define d7 12
#define m7 13
#define M7 14

//
// Note & Interval Names
//
static const std::array<const char *, 15> NOTE_NAMES_15 =
    // 0        1    2     3     4     5     6         7    8     9    10   11
    // 12    13   14
    {n_C, n_Cs_Db, n_D,  n_Ds, n_Eb, n_E,  n_F, n_Fs_Gb,
     n_G, n_Gs,    n_Ab, n_A,  n_As, n_Bb, n_B};

static const std::array<const char *, 15> INTERVAL_NAMES_15 =
    // 0     1     2     3     4     5     6     7     8     9    10    11    12
    // 13    14
    {i_P1, i_m2, i_M2, i_A2, i_m3, i_M3, i_P4, i_TT,
     i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7};

// clang-format off
    
// Modes of the Diatonic Scale in 15 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> IONIAN_D15                 = {P1, M2, M3, P4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> DORIAN_D15                 = {P1, M2, m3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> PHRYGIAN_D15               = {P1, m2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_D15                 = {P1, M2, M3, A4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> MIXOLYDIAN_D15             = {P1, M2, M3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> AEOLIAN_D15                = {P1, M2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_D15                = {P1, m2, m3, P4, d5, m6, m7};

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
#undef A6
#undef d7
#undef m7
#undef M7

} // namespace Music
