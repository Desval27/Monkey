/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Music.h
 * @brief 
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

// #include <ostream>
#include "MusicConst.h"

namespace Music 
{
enum class NoteValue : int
{
    None         = 0,
    Whole        = PPQN * 4,
    Half         = PPQN * 2,
    Quarter      = PPQN,
    Eighth       = PPQN / 2,
    Sixteenth    = PPQN / 4,
    ThirtySecond = PPQN / 8,
#if PPQN >= 32
    SixtyFourth = PPQN / 16,
#endif
#if PPQN >= 64
    // I know...getting ubsurd
    HundredTwentyEightNote = PPQN / 32,
#endif
#if PPQN >= 96
    TwoHundredFiftySixthNote = PPQN / 64,
#endif
};

/**
   * @brief Adds a pulse offset to a note value.
   *
   * @param lhs Base note value.
   * @param rhs Pulse offset to add.
   * @return NoteValue Resulting note value.
   */
inline NoteValue operator+(const NoteValue &lhs, const NoteValue &rhs)
{
    return static_cast<NoteValue>(static_cast<int>(lhs)
                                  + static_cast<int>(rhs));
}

inline int operator+(const NoteValue &lhs, int rhs)
{ return static_cast<int>(lhs) + rhs; }

inline int operator+(int lhs, const NoteValue &rhs)
{ return lhs + static_cast<int>(rhs); }

inline int operator-(const NoteValue &lhs, const NoteValue &rhs)
{ return static_cast<int>(lhs) - static_cast<int>(rhs); }

inline int operator-(const NoteValue &lhs, int rhs)
{ return static_cast<int>(lhs) - rhs; }

inline NoteValue operator*(const NoteValue &lhs, int rhs)
{ return static_cast<NoteValue>(static_cast<int>(lhs) * rhs); }

inline NoteValue operator*(int lhs, const NoteValue &rhs)
{ return static_cast<NoteValue>(lhs * static_cast<int>(rhs)); }

inline NoteValue operator/(const NoteValue &lhs, int rhs)
{ return static_cast<NoteValue>(static_cast<int>(lhs) / rhs); }

inline int operator%(const NoteValue &lhs, int rhs)
{ return static_cast<int>(lhs) % rhs; }

inline NoteValue &operator+=(NoteValue &lhs, const NoteValue &rhs)
{
    lhs = static_cast<NoteValue>(static_cast<int>(lhs) + static_cast<int>(rhs));
    return lhs;
}

inline NoteValue &operator+=(NoteValue &lhs, int rhs)
{
    lhs = static_cast<NoteValue>(static_cast<int>(lhs) + rhs);
    return lhs;
}

inline NoteValue &operator-=(NoteValue &lhs, const NoteValue &rhs)
{
    lhs = static_cast<NoteValue>(static_cast<int>(lhs) - static_cast<int>(rhs));
    return lhs;
}

inline NoteValue &operator-=(NoteValue &lhs, int rhs)
{
    lhs = static_cast<NoteValue>(static_cast<int>(lhs) - rhs);
    return lhs;
}

inline NoteValue &operator*=(NoteValue &lhs, int rhs)
{
    lhs = static_cast<NoteValue>(static_cast<int>(lhs) * rhs);
    return lhs;
}

inline NoteValue &operator/=(NoteValue &lhs, int rhs)
{
    lhs = static_cast<NoteValue>(static_cast<int>(lhs) / rhs);
    return lhs;
}

inline NoteValue &operator%=(NoteValue &lhs, int rhs)
{
    lhs = static_cast<NoteValue>(static_cast<int>(lhs) % rhs);
    return lhs;
}

// inline std::ostream &operator<<(std::ostream &os, const NoteValue &value)
// {
//     os << static_cast<int>(value);
//     return os;
// }

} // namespace Music
