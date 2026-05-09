/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file ScaleNameLabels.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

// Yes....modes are not scales but they are close enough for our purposes and
// it is easier to maintain a single list of names here.

namespace music {
// Modes of the Pentatonic Scale
static const char* s_MAJOR_PENTATONIC = "MAJOR PENTATONIC";
static const char* s_SUSPENDED_PENTATONIC = "SUSPENDED PENTATONIC";
static const char* s_BLUES_MINOR_PENTATONIC = "BLUES MINOR";
static const char* s_BLUES_MAJOR_PENTATONIC = "BLUES MAJOR";
static const char* s_MINOR_PENTATONIC = "MINOR_PENTATONIC";

// Modes of the Diatonic Scale
static const char* s_IONIAN = "IONIAN";
static const char* s_DORIAN = "DORIAN";
static const char* s_PHRYGIAN = "PHRYGIAN";
static const char* s_LYDIAN = "LYDIAN";
static const char* s_MIXOLYDIAN = "MIXOLYDIAN";
static const char* s_AEOLIAN = "AEOLIAN";
static const char* s_LOCRIAN = "LOCRIAN";

// Modes of the Harmonic Minor Scale
static const char* s_HARMONIC_MINOR = "HARMONIC MINOR";
static const char* s_LOCRIAN_s6 = "LOCRIAN #6";
static const char* s_IONIAN_s5 = "IONIAN #5";
static const char* s_DORIAN_s4 = "DORIAN #4";
static const char* s_PHYRGIAN_DOMINANT = "PHYR DOMINANT";
static const char* s_LYDIAN_s2 = "LYDIAN #2";
static const char* s_ULTRALOCRIAN = "ULTRALOCRIAN";

// Modes of the Melodic Minor Scale
static const char* s_MELODIC_MINOR = "MELODIC MINOR";
static const char* s_DORIAN_b2 = "DORIAN b2";
static const char* s_LYDIAN_AUG = "LYDIAN AUGMENTED";
static const char* s_LYDIAN_DOMINANT = "LYDIAN DOMINANT";
static const char* s_MIXOLYDIAN_b6 = "MIXOLYDIAN b6";
static const char* s_LOCRIAN_s2 = "LOCRIAN #2";
static const char* s_SUPER_LOCRIAN = "SUPER LOCRIAN";

// Modes of the Harmonic Major Scale
static const char* s_HARMONIC_MAJOR = "HARMONIC MAJOR";
static const char* s_DORIAN_b5 = "DORIAN b5";
static const char* s_PHRYGIAN_b4 = "PHRYGIAN b4";
static const char* s_LYDIAN_b3 = "LYDIAN b3";
static const char* s_MIXOLYDIAN_b2 = "MIXOLYDIAN b2";
static const char* s_LYDIAN_AUG_s2 = "LYDIAN AUG #2";
static const char* s_LOCRIAN_b7 = "LOCRIAN b7";

// Modes of the Double Harmonic Major Scale
static const char* s_DOUBLE_HARMONIC = "DBL HARMONIC MAJOR";
static const char* s_LYDIAN_b6_b7 = "LYDIAN b6b7";
static const char* s_ULTRAPHRYGIAN = "ULTRAPHRYGIAN";
static const char* s_HUNGARIAN_MINOR = "HUNGARIAN MINOR";
static const char* s_ORIENTAL = "ORIENTAL";
static const char* s_IONIAN_s2_s5 = "IONIAN #2#5";
static const char* s_LOCRIAN_bb3_bb7 = "LOCRIAN bb3bb7";
} // namespace Music
