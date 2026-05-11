/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file music_setup.hpp
 * @brief Collecting various music system components into a single group.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */

#pragma once

#include <Music/music_config.hpp>
#include <Music/music_tables.hpp>
#include <Music/note.hpp>
#include <Music/scale_map.hpp>
#include <Music/temperament.hpp>
#include <Music/time_signature.hpp>

namespace music {

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam MAX_DEGREES
/// @tparam SCALE_DEGREES
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES>
struct Setup
{
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param beats
  /// @param beatValue
  /// @param degreesPerPeriod
  /// @param periodRatio
  Setup(int beats,
        NoteValue beatValue,
        uint16_t degreesPerPeriod,
        float periodRatio)
    : time_signature(beats, beatValue)
    , tuning_reference(BASE_HZ, Note_M6, 0)
    , temperament(degreesPerPeriod, periodRatio)
    , bars(8)
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  Setup()
    : Setup(4, NoteValue::Quarter, TWELVE_TONE, OCTAVE_DOUBLE)
  {
  }

  const TimeSignature time_signature;
  const TuningReference tuning_reference;

  Temperament<MAX_DEGREES> temperament;
  ScaleMap<SCALE_DEGREES> scale_map;
  int bars;
};

} // namespace Music
