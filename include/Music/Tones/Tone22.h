/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Tone22.h
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

namespace Music
{
// clang-format off

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 22 Tone Temperaments
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// For mapping 22 tone scales
// 22 EDO - Full Chromatic Detail
#define P1  0  
#define m2  1  // Minor Second
#define n2  2  // Neutral Second
#define dM2 3  // Minor Tone / "Small" Whole Tone (10:9 approx)
#define M2  4  // Major Second / "Large" Whole Tone (9:8 approx)
#define m3  5  // Subminor Third (7:6 approx)
#define n3  6  // Neutral Third
#define M3  7  // Just Major Third (5:4 approx)
#define sM3 8  // Supermajor Third (9:7 approx)
#define P4  9  // Perfect Fourth
#define A4  10 // Augmented Fourth
#define TT  11 // Tritone (Midpoint)
#define d5  12 // Diminished Fifth
#define P5  13 // Perfect Fifth
#define m6  14 // Subminor Sixth
#define n6  15 // Neutral Sixth
#define M6  16 // Just Minor Sixth (8:5 approx)
#define sM6 17 // Major Sixth
#define m7  18 // Septimal Minor Seventh (7:4 approx)
#define n7  19 // Neutral Seventh
#define jM7 20 // Just Major Seventh (15:8 approx)
#define M7  21 // Supermajor Seventh

//
// Note & Interval Names
//
static const std::array<const char *, 22> NOTE_NAMES_22 = { 
    n_C, n_Cup_Db, n_Csdn_Dbup, n_Cs_Ddn, 
    n_D, n_Dup_Eb, n_Dsdn_Ebup, n_Ds_Edn, 
    n_E, 
    n_F, n_Fup_Gb, n_Fsdn_Gbup, n_Fs_Gdn, 
    n_G, n_Gup_Ab, n_Gsdn_Abup, n_Gs_Adn,
    n_A, n_Aup_Bb, n_Asdn_Bbup, n_As_Bdn, 
    n_B, 
};

static const std::array<const char *, 22> INTERVAL_NAMES_22 = { 
    i_P1, 
    i_m2, i_n2, i_dM2, i_M2,
    i_m3, i_n3, i_M3, i_sM3,
    i_P4, i_A4, i_TT,
    i_d5, i_P5,
    i_m6, i_n6, i_M6, i_sM6,
    i_m7, i_n7, i_jM7, i_M7
};

// Modes of the Diatonic Scale in 22 Tone Temperament.
static constexpr DegreeMap<HEPATONIC> IONIAN_D22            = {P1, M2, M3, P4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> DORIAN_D22            = {P1, M2, m3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> PHRYGIAN_D22          = {P1, m2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LYDIAN_D22            = {P1, M2, M3, A4, P5, M6, M7};
static constexpr DegreeMap<HEPATONIC> MIXOLYDIAN_D22        = {P1, M2, M3, P4, P5, M6, m7};
static constexpr DegreeMap<HEPATONIC> AEOLIAN_D22           = {P1, M2, m3, P4, P5, m6, m7};
static constexpr DegreeMap<HEPATONIC> LOCRIAN_D22           = {P1, m2, m3, P4, d5, m6, m7};

#undef P1
#undef m2
#undef n2
#undef dM2
#undef M2
#undef m3
#undef n3
#undef M3
#undef sM3
#undef P4
#undef A4
#undef TT
#undef d5
#undef P5
#undef m6
#undef n6
#undef M6
#undef sM6
#undef m7
#undef n7
#undef jM7
#undef M7

// clang-format on

} // namespace Music
