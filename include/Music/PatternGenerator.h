/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Music.h
 * @brief General Music Definitions.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#ifdef DEBUG_COUT
#include <iostream>
#endif

#include <cstddef>

#include <Monkey.h>
#include <Music/MusicConfig.h>
#include <Music/NoteValue.h>
#include <Music/TimeSignature.h>
#include <Music/EventSet.h>

namespace Music
{
  template <std::size_t MAX_EVENTS>
  std::size_t BuildEuclid(int k, int n, int r, PatternEventSet<MAX_EVENTS> &pattern);

  //
  // Different Pattern Generators
  //

  /**
   * @brief A pattern generator that produces no events. Useful for testing and as a default generator.
   * The GeneratePattern function simply clears the provided pattern and returns a count of zero, indicating
   */
  template <std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  class NullPatternGenerator
  {
  public:
    static std::size_t GeneratePattern(const TimeSignature &ts, int bars,
                                  float density, NoteValue granularity,
                                  PatternEventSet<MAX_EVENTS> &pattern)
    {
      pattern.Clear();
      return pattern.Count();
    }
  };

  /**
   * @brief A pattern generator that produces random events based on a specified density. Each potential event slot is filled with an event with a probability equal to the density parameter.
   * The GeneratePattern function calculates the total number of event slots based on the time signature, number of bars, and granularity, and then randomly fills those slots according to the specified density. This generator can produce patterns with varying levels of activity, from sparse to dense, depending on the density value provided.
   */
  template <std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  class SimpleRandomPatternGenerator
  {
  public:
    static std::size_t GeneratePattern(const TimeSignature &ts, int bars,
                                  float density, NoteValue granularity,
                                  PatternEventSet<MAX_EVENTS> &pattern)
    {
      if (pattern.Capacity() == 0 || bars == 0 || ts.beatValue == NoteValue::None || granularity == NoteValue::None)
        return 0; // Sanity check

      
      std::size_t t = bars * ts.beats * ts.beatValue;
      std::size_t n = t / granularity;
      std::size_t r = t % granularity;
      #ifdef DEBUG_COUT
      std::clog << "Simple Pattern: " << t << "/" << granularity << " slots/granularity = " << n <<  " events with " << r << " slots remaining" << std::endl;
      #endif

      std::size_t eventsToEmit = min(n, pattern.Capacity());
      for (std::size_t i = 0; i < eventsToEmit; i++)
      {
        pattern.Emplace(randomRange(0.0f, 1.0f) < density);
      }
      return pattern.Count();
    }
  };

  /**
   * @brief A pattern generator that produces Euclidean rhythms based on the specified density. The generator calculates the number of events to emit based on the density and then uses the Euclidean algorithm to distribute those events as evenly as possible across the available slots defined by the time signature and granularity.
   * The GeneratePattern function first determines the total number of event slots based on the time signature, number of bars, and granularity. It then calculates the number of events to emit based on
   */
  template <std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  class EuclidianPatternGenerator
  {
  public:
    static std::size_t GeneratePattern(const TimeSignature &ts, int bars,
                                  float density, NoteValue granularity,
                                  PatternEventSet<MAX_EVENTS> &pattern)
    {
      if (pattern.Capacity() == 0 || bars == 0 || ts.beatValue == NoteValue::None || granularity == NoteValue::None)
        return 0; // Sanity check

      int t = bars * ts.beats * ts.beatValue;
      int n = t / granularity;
      int r = t % granularity;
      int k = n * density;
      #ifdef DEBUG_COUT
      std::clog << "Eulclidian Pattern: " << t << "/" << granularity << " slots/granularity = " << k <<  " events out of " << n << " with " << r << " slots remaining" << std::endl;
      #endif
      
      return BuildEuclid(k, n, 1, pattern);
    }
  };

} // namespace Music
