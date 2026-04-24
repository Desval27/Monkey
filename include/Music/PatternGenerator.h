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

#include <Music/MusicConfig.h>
#include <Music/NoteValue.h>
#include <Music/TimeSignature.h>
#include <Music/EventSet.h>

namespace Music
{
  // Different Pattern Generators
  template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  class NullPatternGenerator
  {
  public:
    static size_t GeneratePattern(const TimeSignature &ts, int bars,
                                  float density, NoteValue granularity,
                                  PatternEventSet<MAX_EVENTS> &pattern)
    {
      pattern.Clear();
      return pattern.Count();
    }
  };

  template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  class SimpleRandomPatternGenerator
  {
  public:
    static size_t GeneratePattern(const TimeSignature &ts, int bars,
                                  float density, NoteValue granularity,
                                  PatternEventSet<MAX_EVENTS> &pattern)
    {
      if (pattern.Capacity() == 0 || bars == 0 || ts.beatValue == NoteValue::None || granularity == NoteValue::None)
        return 0; // Sanity check

      
      size_t t = bars * ts.beats * ts.beatValue;
      size_t n = t / granularity;
      size_t r = t % granularity;
      #ifdef DEBUG_COUT
      std::cout << "Simple Pattern: " << t << "/" << granularity << " slots/granularity = " << n <<  " events with " << r << " slots remaining" << std::endl;
      #endif

      size_t eventsToEmit = min(n, pattern.Capacity());
      for (size_t i = 0; i < eventsToEmit; i++)
      {
        pattern.Emplace(randomRange(0.0f, 1.0f) < density);
      }
      return pattern.Count();
    }
  };

  template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  class EuclidianPatternGenerator
  {
  public:
    static size_t GeneratePattern(const TimeSignature &ts, int bars,
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
      std::cout << "Eulclidian Pattern: " << t << "/" << granularity << " slots/granularity = " << k <<  " events out of " << n << " with " << r << " slots remaining" << std::endl;
      #endif
      
      return BuildEuclid(k, n, 1, pattern);
    }
  };

} // namespace Music
