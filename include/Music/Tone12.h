#pragma once

#include <Music/Music.h>

namespace Music
{
    // clang-format off

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

// Modes of the Diatonic Scale in 12 Tone Temperament.
static const Degree IONIAN_D12[]                 = {P1, M2, M3, P4, P5, M6, M7};
static const Degree DORIAN_D12[]                 = {P1, M2, m3, P4, P5, M6, m7};
static const Degree PHRYGIAN_D12[]               = {P1, m2, m3, P4, P5, m6, m7};
static const Degree LYDIAN_D12[]                 = {P1, M2, M3, A4, P5, M6, M7};
static const Degree MIXOLYDIAN_D12[]             = {P1, M2, M3, P4, P5, M6, m7};
static const Degree AEOLIAN_D12[]                = {P1, M2, m3, P4, P5, m6, m7};
static const Degree LOCRIAN_D12[]                = {P1, m2, m3, P4, d5, m6, m7};
// Modes of the Harmonic Minor Scale in 12 Tone Temperament.
static const Degree HARMONIC_MINOR_D12[]         = {P1, M2, m3, P4, P5, m6, M7};
static const Degree LOCRIAN_s6_D12[]             = {P1, m2, m3, P4, d5, M6, m7};
static const Degree IONIAN_s5_D12[]              = {P1, M2, M3, P4, m6, M6, M7};
static const Degree DORIAN_s4_D12[]              = {P1, M2, m3, A4, P5, M6, m7};
static const Degree PHRYGIAN_DOMINANT_D12[]      = {P1, m2, M3, P4, P5, m6, m7};
static const Degree LYDIAN_s2_DEGREES12[]        = {P1, m3, M3, A4, P5, M6, M7};
static const Degree ULTRALOCRIAN_D12[]           = {P1, m2, m3, M3, d5, m6, M6};
// Modes of the Melodic Minor Scale in 12 Tone Temperament.
static const Degree MELODIC_MINOR_D12[]          = {P1, M2, m3, P4, P5, M6, M7};
static const Degree DORIAN_b2_D12[]              = {P1, m2, m3, P4, P5, M6, m7};
static const Degree LYDIAN_AUG_D12[]             = {P1, M2, M3, A4, A5, M6, M7};
static const Degree LYDIAN_DOMINANT_D12[]        = {P1, M2, M3, A4, P5, M6, m7};
static const Degree MIXOLYDIAN_b6_D12[]          = {P1, M2, M3, P4, P5, m6, m7};
static const Degree LOCRIAN_s2_D12[]             = {P1, M2, m3, P4, d5, m6, m7};
static const Degree SUPER_LOCRIAN_D12[]          = {P1, m2, m3, d4, d5, m6, m7};     
// Modes of the Harmonic Major Scale in 12 Tone Temperament.
static const Degree HARMONIC_MAJOR_D12[]         = {P1, M2, M3, P4, P5, m6, M7};
static const Degree DORIAN_b5_D12[]              = {P1, M2, m3, P4, d5, m6, m7};
static const Degree PHRYGIAN_b4_D12[]            = {P1, m2, m3, d4, P5, m6, m7};
static const Degree LYDIAN_b3_D12[]              = {P1, M2, m3, A4, P5, M6, M7};
static const Degree MIXOLYDIAN_b2_D12[]          = {P1, m2, M3, P4, P5, M6, m7};
static const Degree LYDIAN_AUG_s2_D12[]          = {P1, A2, M3, A4, A5, M6, M7};
static const Degree LOCRIAN_b7_D12[]             = {P1, m2, m3, P4, d5, m6, d7};
// Modes of the Double Harmonic Scale in 12 Tone Temperament.
static const Degree DOUBLE_HARMONIC_D12[]        = {P1, m2, M3, P4, P5, m6, M7};
static const Degree LYDIAN_b6_b7_D12[]           = {P1, M2, M3, A4, P5, m6, m7};
static const Degree ULTRAPHRYGIAN_D12[]          = {P1, m2, m3, d4, P5, m6, d7};
static const Degree HUNGARIAN_MINOR_D12[]        = {P1, M2, m3, A4, P5, m6, M7};
static const Degree ORIENTAL_D12[]               = {P1, m2, M3, P4, A5, M6, M7};
static const Degree IONIAN_s2_s5_D12[]           = {P1, A2, M3, P4, A5, M6, M7};
static const Degree LOCRIAN_bb3_bb7_D12[]        = {P1, m2, d3, P4, d5, m6, d7};

// Common pentatonic maps in 12-EDO.
static const Degree MAJOR_PENTATONIC_D12[]       = {P1, M2, M3, P5, M6};
static const Degree SUSPENDED_PENTATONIC_D12[]   = {P1, M2, P4, P5, m7};
static const Degree MINOR_BLUES_PENTATONIC_D12[] = {P1, m3, P4, m6, m7};
static const Degree MAJOR_BLUES_PENTATONIC_D12[] = {P1, M2, P4, P5, M6};
static const Degree MINOR_PENTATONIC_D12[]       = {P1, m3, P4, P5, m7};

// Common hexatonic maps in 12-EDO.
static const Degree WHOLE_TONE_HEXATONIC_D12[] = {P1, M2, M3, A4, A5, M6};
static const Degree BLUES_HEXATONIC_D12[]      = {P1, m3, P4, d5, P5, m7};
static const Degree AUGMENTED_HEXATONIC_D12[]  = {P1, m3, M3, A4, A5, M6};

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

static_assert(ArrayLen(IONIAN_D12) == 7, "IONIAN_D12 array length mismatch");
static_assert(ArrayLen(DORIAN_D12) == 7, "DORIAN_D12 array length mismatch");
static_assert(ArrayLen(PHRYGIAN_D12) == 7, "PHRYGIAN_D12 array length mismatch");
static_assert(ArrayLen(LYDIAN_D12) == 7, "LYDIAN_D12 array length mismatch");
static_assert(ArrayLen(MIXOLYDIAN_D12) == 7, "MIXOLYDIAN_D12 array length mismatch");
static_assert(ArrayLen(AEOLIAN_D12) == 7, "AEOLIAN_D12 array length mismatch");
static_assert(ArrayLen(LOCRIAN_D12) == 7, "LOCRIAN_D12 array length mismatch");

    // clang-format on

} // namespace Music