/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file time_signature.hpp
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/music_types.hpp>
#include <Music/note_value.hpp>

namespace music {

/**
 * @brief Broad classification of a time signature.
 */
enum class TimeSignatureType : uint8_t
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
 * @brief Represents a musical time signature, which defines the rhythmic
 * structure of a piece of music. It consists of two main components: the
 * number of beats per bar and the note value that receives one beat. The
 * time signature determines how music is counted and organized into
 * measures, influencing the feel and flow of the music.
 *
 * The `TimeSignature` struct provides methods to classify the time
 * signature type, calculate the denominator based on the beat value, and
 * determine the total number of pulses in a bar. This allows for flexible
 * handling of various time signatures, including common, simple,
 * compound, and irregular meters.
 */
struct TimeSignature
{
  /**
   * @brief Number of beats in each bar.
   */
  int beats; // e.g., 4 for 4/4 time, 6 for 6/8 time
  /**
   * @brief Note value that counts as one beat.
   */
  NoteValue beatValue; // e.g., Quarter for 4/4 time, Eighth for 6/8 time

  /**
   * @brief Constructs a time signature from beats per bar and beat unit.
   *
   * @param b Number of beats in each bar.
   * @param v Note value that receives one beat.
   */
  TimeSignature(int b, NoteValue v)
    : beats(b)
    , beatValue(v) {};

  /**
   * @brief Constructs a common 4/4 time signature.
   */
  TimeSignature()
    : TimeSignature(4, NoteValue::Quarter) {};

  /**
   * @brief Copy Constructor
   */
  TimeSignature(const TimeSignature& other) = default;

  /**
   * @brief Classifies the time signature.
   *
   * @return TimeSignatureType Derived type for the current meter.
   */
  [[nodiscard]] TimeSignatureType get_type() const
  {
    if (beats == 4 && beatValue == NoteValue::Quarter) {
      return TimeSignatureType::Common;
    }

    if ((beats == 3 || beats == 2) && beatValue == NoteValue::Quarter) {
      return TimeSignatureType::Simple;
    }

    if ((beats == 6 || beats == 9 || beats == 12) &&
        beatValue == NoteValue::Eighth) {
      return TimeSignatureType::Compound;
    }

    return TimeSignatureType::Irregular;
  }

  [[nodiscard]] bool in_threes() const
  {
    // Generally considered (for now) to be potentially considered
    // to be in threes if the number of beats is evenly divisible
    // by three and the denominator is evenly divisible by two.
    return (beats % 3 == 0) && (get_denominator() % 2 == 0);
  }

  /**
   * @brief Calculates the denominator of the time signature.
   * The denominator is derived from the beat value and represents the note
   * value that receives one beat. For example, if the beat value is a quarter
   * note, the denominator would be 4 (as in 4/4 time).
   * @return int The denominator of the time signature, or 0 if the beat value
   * is invalid.
   */
  [[nodiscard]] int get_denominator() const
  {
    if (static_cast<int>(beatValue) != 0) {
      return (4 * PPQN) / static_cast<int>(beatValue);
    }
    return 0; // Avoid division by zero, though this case should be handled
              // more gracefully in practice.
  }

  /**
   * @brief Calculates the total number of pulses in one bar.
   * @return int Pulses contained in a full bar.
   */
  [[nodiscard]] int get_pulses_per_bar() const
  {
    return (beats * static_cast<int>(beatValue));
  }
};

} // namespace Music
