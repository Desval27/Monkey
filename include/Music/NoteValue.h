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

#include <Music/MusicTypes.h>

namespace Music
{
    enum NoteValue : int
    {
        None = 0,

        DottedDottedWhole = (PPQN * 4) + (PPQN * 2) + (PPQN * 1),
        DottedWhole = (PPQN * 4) + (PPQN * 2),
        Whole = PPQN * 4,

        DottedDottedHalf = (PPQN * 2) + (PPQN * 1) + (PPQN / 2),
        DottedHalf = (PPQN * 2) + (PPQN * 1),
        Half = PPQN * 2,

        DottedDottedQuarter = (PPQN) + (PPQN / 2) + (PPQN / 4),
        DottedQuarter = (PPQN) + (PPQN / 2),
        Quarter = PPQN,

        DottedDottedEighth = (PPQN / 2) + (PPQN / 4) + (PPQN / 8),
        DottedEighth = (PPQN / 2) + (PPQN / 4),
        Eighth = PPQN / 2,

        DottedDottedSixteenth = (PPQN / 4) + (PPQN / 8) + (PPQN / 16),
        DottedSixteenth = (PPQN / 4) + (PPQN / 8),
        Sixteenth = PPQN / 4,

        DottedThirtySecond = (PPQN / 8) + (PPQN / 16),
        ThirtySecond = PPQN / 8,

#if PPQN >= 32
        DottedDottedThirtySecond = (PPQN / 8) + (PPQN / 16) + (PPQN / 32),
        DottedSixtyFourth = (PPQN / 16) + (PPQN / 32),
        SixtyFourth = PPQN / 16,
#endif
#if PPQN >= 64
        // I know...getting ubsurd
        DottedHundredTwentyEightNote = (PPQN / 32) + (PPQN / 64),
        HundredTwentyEightNote = PPQN / 32,
#endif
#if PPQN >= 96
        DottedTwoHundredFiftySixthNote = (PPQN / 64) + (PPQN / 128),
        TwoHundredFiftySixthNote = PPQN / 64,
#endif
    };
#if false

    /**
     * @brief Adds a pulse offset to a note value.
     *
     * @param lhs Base note value.
     * @param rhs Pulse offset to add.
     * @return NoteValue Resulting note value.
     */
    inline NoteValue operator+(const NoteValue &lhs, const NoteValue &rhs)
    {
        return static_cast<NoteValue>(static_cast<int>(lhs) + static_cast<int>(rhs));
    }

    inline int operator+(const NoteValue &lhs, int rhs)
    {
        return static_cast<int>(lhs) + rhs;
    }

    inline int operator+(int lhs, const NoteValue &rhs)
    {
        return lhs + static_cast<int>(rhs);
    }

    inline int operator-(const NoteValue &lhs, const NoteValue &rhs)
    {
        return static_cast<int>(lhs) - static_cast<int>(rhs);
    }

    inline int operator-(const NoteValue &lhs, int rhs)
    {
        return static_cast<int>(lhs) - rhs;
    }

    inline NoteValue operator*(const NoteValue &lhs, int rhs)
    {
        return static_cast<NoteValue>(static_cast<int>(lhs) * rhs);
    }

    inline NoteValue operator*(int lhs, const NoteValue &rhs)
    {
        return static_cast<NoteValue>(lhs * static_cast<int>(rhs));
    }

    inline NoteValue operator/(const NoteValue &lhs, int rhs)
    {
        return static_cast<NoteValue>(static_cast<int>(lhs) / rhs);
    }

    inline int operator%(const NoteValue &lhs, int rhs)
    {
        return static_cast<int>(lhs) % rhs;
    }

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
#endif

    inline bool IsNoteValueWeird(const NoteValue v)
    {
        // Crude but workable for now.
        switch (v)
        {
#if PPQN >= 32
        case NoteValue::SixtyFourth:
        case NoteValue::DottedSixtyFourth:
#endif
        case NoteValue::ThirtySecond:
        case NoteValue::DottedThirtySecond:
        case NoteValue::Sixteenth:
        case NoteValue::DottedSixteenth:
        case NoteValue::Eighth:
        case NoteValue::DottedEighth:
        case NoteValue::Quarter:
        case NoteValue::DottedQuarter:
        case NoteValue::Half:
        case NoteValue::DottedHalf:
        case NoteValue::Whole:
            // case NoteValue::DottedWhole:
            return false;
        default:
            return true;
        }
    }

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

} // namespace Music
