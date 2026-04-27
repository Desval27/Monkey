/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Tone17.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief 
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

namespace Music
{
///////////////////////////////////////////////////////////////////////////////////////////////////////
// 17 Tone Temperaments
//    0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16
// i_P1, i_A1, i_m2, i_M2, i_A2, i_m3, i_M3, i_P4, i_A4, i_d5, i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7
///////////////////////////////////////////////////////////////////////////////////////////////////////
// For mapping 17 tone scales
#if false
// 17 EDO - Full Chromatic
#define P1  0  // Perfect Unison
#define m2  1  // Minor Second (Diatonic semitone)
#define A1  2  // Augmented Unison (Chromatic semitone)
#define M2  3  // Major Second
#define m3  4  // Minor Third
#define A2  5  // Augmented Second
#define M3  6  // Major Third
#define P4  7  // Perfect Fourth
#define d5  8  // Diminished Fifth
#define A4  9  // Augmented Fourth
#define P5  10 // Perfect Fifth
#define m6  11 // Minor Sixth
#define A5  12 // Augmented Fifth
#define M6  13 // Major Sixth
#define m7  14 // Minor Seventh
#define A6  15 // Augmented Sixth
#define M7  16 // Major Seventh
#endif

#define P1 0
#define A1 1
#define m2 2
#define M2 3
#define A2 4
#define d3 4
#define m3 5
#define M3 6
#define d4 6
#define P4 7
#define A4 8
#define d5 9
#define P5 10
#define A5 11
#define d6 11
#define m6 12
#define M6 13
#define A6 14
#define d7 14
#define m7 15
#define M7 16

//
// Note & Interval Names
//
static const std::array<const char *, 17> NOTE_NAMES_17 =
    {n_C, n_Cs, n_Db, n_D, n_Ds, n_Eb, n_E, n_F, n_Fs, n_Gb, n_G, n_Gs, n_Ab, n_A, n_As, n_Bb, n_B};

static const std::array<const char *, 17> INTERVAL_NAMES_17 =
    {i_P1, i_A1, i_m2, i_M2, i_A2, i_m3, i_M3, i_P4, i_A4, i_d5, i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7};

// clang-format off

// Modes of the Diatonic Scale in 17 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> IONIAN_D17                 = {P1, M2, M3, P4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> DORIAN_D17                 = {P1, M2, m3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> PHRYGIAN_D17               = {P1, m2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_D17                 = {P1, M2, M3, A4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> MIXOLYDIAN_D17             = {P1, M2, M3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> AEOLIAN_D17                = {P1, M2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_D17                = {P1, m2, m3, P4, d5, m6, m7};

// clang-format on

#undef P1
#undef A1
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
