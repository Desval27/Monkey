/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file TimeSignature.h
 * @brief
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

    /**
     * @brief Broad classification of a time signature.
     */
    enum class TimeSignatureType : int
    {
        Common,    // 4/4  Technically simple but we break it out separately here
        Simple,    // 3/4, 2/4
        Compound,  // 6/8, 9/8, 12/8
        Irregular, // 5/4, 7/8, etc.
        // These are not categorized yet.
        Complex,
        Mixed,
        Additive,
        Irrational,
    };

    /**
     * @brief
     *
     */
    struct TimeSignature
    {
        /**
         * @brief Number of beats in each bar.
         */
        int beats; // e.g., 4 for 4/4 time
        /**
         * @brief Note value that counts as one beat.
         */
        NoteValue beatValue; // e.g., Duration::Quarter for 4/4 time

        /**
         * @brief Constructs a time signature from beats per bar and beat unit.
         *
         * @param b Number of beats in each bar.
         * @param v Note value that receives one beat.
         */
        TimeSignature(int b, NoteValue v) : beats(b), beatValue(v) {};

        /**
         * @brief Constructs a common 4/4 time signature.
         */

        TimeSignature() : TimeSignature(4, NoteValue::Quarter) {}
        /**
         * @brief Copy Constructor
         */

        TimeSignature(const TimeSignature &other)
            : beats(other.beats), beatValue(other.beatValue)
        {
        }

        /**
         * @brief Classifies the time signature.
         *
         * @return TimeSignatureType Derived type for the current meter.
         */
        TimeSignatureType GetType() const
        {
            if (beats == 4 && beatValue == NoteValue::Quarter)
                return TimeSignatureType::Common;
            else if ((beats == 3 || beats == 2) && beatValue == NoteValue::Quarter)
                return TimeSignatureType::Simple;
            else if ((beats == 6 || beats == 9 || beats == 12) && beatValue == NoteValue::Eighth)
                return TimeSignatureType::Compound;
            else
                return TimeSignatureType::Irregular;
        }

        int GetDenominator() const
        {
            if (static_cast<int>(beatValue) != 0)
                return (4 * PPQN) / static_cast<int>(beatValue);
            else
                return 0; // Avoid division by zero, though this case should be handled more gracefully in practice.
        }

        /**
         * @brief Calculates the total number of pulses in one bar.
         *
         * @return int Pulses contained in a full bar.
         */
        int PulsesPerBar() const { return (beats * static_cast<int>(beatValue)); }
    };

} // namespace Music