/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file MusicConst.h
 * @brief Simple Music Constants.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/Music.h>

namespace Music
{
/// @brief The number of clock pulses per quarter note.
// This is independent of the beat unit of a time signature.
// Example: If a time signature's beat unit is an eighth note the
// number of pulses for it will still be PPQN / 2.
#define PPQN 24

constexpr float     C4FREQ = 261.6256f;

// for convience for now.
constexpr Note REST    = -1;
constexpr Note Note_P1 = 0;
constexpr Note Note_m2 = 1;
constexpr Note Note_M2 = 2;
constexpr Note Note_m3 = 3;
constexpr Note Note_M3 = 4;
constexpr Note Note_P4 = 5;
constexpr Note Note_TT = 6;
constexpr Note Note_P5 = 7;
constexpr Note Note_m6 = 8;
constexpr Note Note_M6 = 9;
constexpr Note Note_m7 = 10;
constexpr Note Note_M7 = 11;


} // namespace Music
