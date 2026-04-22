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

      int n = (ts.beats * bars) * (static_cast<int>(ts.beatValue) / static_cast<int>(granularity));
      int maxLen = min(n, static_cast<int>(pattern.Capacity()));
      for (int i = 0; i < maxLen; i++)
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

      int n = (ts.beats * bars) *
              (static_cast<int>(ts.beatValue) / static_cast<int>(granularity));
      int k = n * density;
      return BuildEuclid(k, n, 1, pattern);
    }
  };

} // namespace Music
