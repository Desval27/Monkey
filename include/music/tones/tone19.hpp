/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Tone19.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <array>

#include <Music/labels/interval_name_labels.hpp>
#include <Music/labels/note_name_labels.hpp>
#include <Music/music_types.hpp>

namespace music {
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 19 Tone Temperaments
//    0     1     2     3     4     5     6     7     8     9    10    11    12
//    13    14    15    16    17    18
// i_P1, i_A1, i_m2, i_M2, i_A2, i_m3, i_M3, i_A3, i_P4, i_A4, i_d5, i_P5,
// i_A5, i_m6, i_M6, i_A6, i_m7, i_M7, i_A7
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// For mapping 19 tone scales
#define P1 0
#define A1 1
#define m2 2
#define M2 3
#define A2 4
#define d3 4
#define m3 5
#define M3 6
#define A3 7
#define d4 7
#define P4 8
#define A4 9
#define d5 10
#define P5 11
#define A5 12
#define d6 12
#define m6 13
#define M6 14
#define A6 15
#define d7 15
#define m7 16
#define M7 17
#define A7 18

//
// Note & Interval Names
//
static const std::array<const char*, 19> NOTE_NAMES_19 = {
  n_C,  n_Cs, n_Db, n_D,  n_Ds, n_Eb, n_E,  n_Es_Fb, n_F,    n_Fs,
  n_Gb, n_G,  n_Gs, n_Ab, n_A,  n_As, n_Bb, n_B,     n_Bs_Cb
};

static const std::array<const char*, 19> INTERVAL_NAMES_19 = {
  i_P1, i_A1, i_m2, i_M2, i_A2, i_m3, i_M3, i_A3, i_P4, i_A4,
  i_d5, i_P5, i_A5, i_m6, i_M6, i_A6, i_m7, i_M7, i_A7
};

// clang-format off

// Modes of the Diatonic Scale in 19 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> IONIAN_D19            = {P1, M2, M3, P4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> DORIAN_D19            = {P1, M2, m3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> PHRYGIAN_D19          = {P1, m2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_D19            = {P1, M2, M3, A4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> MIXOLYDIAN_D19        = {P1, M2, M3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> AEOLIAN_D19           = {P1, M2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_D19           = {P1, m2, m3, P4, d5, m6, m7};

// clang-format on

#undef P1
#undef A1
#undef m2
#undef M2
#undef A2
#undef d3
#undef m3
#undef M3
#undef A3
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
#undef A7

} // namespace Music
