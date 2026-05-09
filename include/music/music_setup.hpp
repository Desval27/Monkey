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

#include <music/music_config.hpp>
#include <music/note.hpp>
#include <music/scale_map.hpp>
#include <music/temperament.hpp>
#include <music/time_signature.hpp>

namespace music {

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES>
struct Setup
{
  Setup(int beats,
        NoteValue beatValue,
        uint16_t degreesPerPeriod,
        float periodRatio)
    : timeSignature(beats, beatValue)
    , tuningReference(BASE_HZ, Note_M6, 0)
    , temperament(degreesPerPeriod, periodRatio)
    , bars(8)
  {
  }
  Setup()
    : Setup(4, NoteValue::Quarter, TWELVE_TONE, OCTAVE_DOUBLE)
  {
  }

  const TimeSignature timeSignature;
  const TuningReference tuningReference;

  Temperament<MAX_DEGREES> temperament;
  ScaleMap<SCALE_DEGREES> scaleMap;
  int bars;
};

} // namespace Music
