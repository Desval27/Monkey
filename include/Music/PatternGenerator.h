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
#include <TTY.h>
#endif

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include <Monkey.h>
#include <Music/MusicConfig.h>
#include <Music/NoteValue.h>
#include <Music/TimeSignature.h>
#include <Music/EventSet.h>

namespace Music
{
  template <std::size_t MAX_EVENTS>
  std::size_t BuildEuclid(
  	int k, int n, int r, 
	PatternEventSet<MAX_EVENTS> &pattern)
  {
    bool buffer[DEF_MAX_EVENTS];
    n = clamp(n, 1, static_cast<int>(std::min(pattern.Capacity(), ArrayLen(buffer))));
    k = clamp(k, 0, n);

    for (uint8_t i = 0; i < n; i++)
      pattern.Add(false);

    uint16_t bucket = 0;
    for (uint8_t i = 0; i < n; i++)
    {
      bucket += k;
      if (bucket >= n)
      {
        bucket -= n;
        pattern[i] = true;
      }
    }

    r = (n == 0) ? 0 : (r % n);
    if (r)
    {
      for (uint8_t i = 0; i < n; i++)
        buffer[i] = pattern[i];
      for (uint8_t i = 0; i < n; i++)
      {
        uint8_t src = (i + (n - r)) % n;
        pattern[i] = buffer[src];
      }
    }
    return pattern.Count();
  }

  //
  // Different Pattern Generators
  //

  /**
   * @brief A pattern generator that produces no events. Useful for testing 
   * and as a default generator.
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
   * @brief A pattern generator that produces random events based on a 
   * specified density. Each potential event slot is filled with an event with 
   * a probability equal to the density parameter.
   *
   * The GeneratePattern function calculates the total number of event slots 
   * based on the time signature, number of bars, and granularity, and then 
   * randomly fills those slots according to the specified density. This 
   * generator can produce patterns with varying levels of activity, from 
   * sparse to dense, depending on the density value provided.
   */
  template <std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  class SimpleRandomPatternGenerator
  {
  public:
    static std::size_t GeneratePattern(const TimeSignature &ts, int bars,
                                  float density, NoteValue granularity,
                                  PatternEventSet<MAX_EVENTS> &pattern)
    {
      if (pattern.Capacity() == 0 
	  	|| bars == 0 
		|| ts.beatValue == NoteValue::None 
		|| granularity == NoteValue::None)
        return 0; // Sanity check
      
      std::size_t t = bars * ts.beats * ts.beatValue;
      std::size_t n = t / granularity;
      std::size_t r = t % granularity;
      #ifdef DEBUG_COUT
      std::clog 
        << TTY_FG_RED << "Simple Pattern:" << TTY_RESET << " "
        << t << "/" << granularity 
        << " slots/granularity = " << n 
        <<  " events with " << r 
        << " slots remaining" 
        << std::endl;
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
   * @brief
   */
  template <std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  class InversionPatternGenerator
  {
  public:
    static std::size_t GeneratePattern(const TimeSignature &ts, int bars,
                                  float density, NoteValue granularity,
                                  PatternEventSet<MAX_EVENTS> &pattern)
    {
      if (pattern.Capacity() == 0 
	  	|| bars == 0 
		|| ts.beatValue == NoteValue::None 
		|| granularity == NoteValue::None
		|| pattern.Count() == 0)
        return 0; // Sanity check
      
      std::size_t t = bars * ts.beats * ts.beatValue;
      std::size_t n = t / granularity;
      std::size_t r = t % granularity;
      #ifdef DEBUG_COUT
      std::clog 
        << TTY_FG_RED << TTY_INVERSE_ON << "Inversion Pattern:" << TTY_RESET << " "
        << t << "/" 
        << granularity << " slots/granularity = " 
        << n <<  " events with " 
        << r << " slots remaining" 
        << std::endl;
      #endif

	  // In case the bars/beats had changed from one version to the other.
      std::size_t eventsToEmit = min(n, pattern.Count());
      for (std::size_t i = 0; i < eventsToEmit; i++)
      {
	  	pattern[i] = !pattern[i];
      }
      return pattern.Count();
    }
  };

  /**
   * @brief A pattern generator that produces Euclidean rhythms based on the 
   * specified density. The generator calculates the number of events to emit 
   * based on the density and then uses the Euclidean algorithm to distribute 
   * those events as evenly as possible across the available slots defined by 
   * the time signature and granularity.
   */
  template <std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
  class EuclidianPatternGenerator
  {
  public:
    static std::size_t GeneratePattern(const TimeSignature &ts, int bars,
                                  float density, NoteValue granularity,
                                  PatternEventSet<MAX_EVENTS> &pattern)
    {
      if (pattern.Capacity() == 0 
	  	|| bars == 0 
		|| ts.beatValue == NoteValue::None 
		|| granularity == NoteValue::None)
        return 0; // Sanity check

      int t = bars * ts.beats * ts.beatValue;
      int n = t / granularity;
      int r = t % granularity;
      int k = n * density;
      #ifdef DEBUG_COUT
      std::clog 
        << TTY_FG_RED << "Eulclidian Pattern:" << TTY_RESET << " "
        << t << "/" << granularity 
        << " slots/granularity = " << k 
        <<  " events out of " << n 
        << " with " << r << " slots remaining" 
        << std::endl;
      #endif
      
      return BuildEuclid(k, n, 1, pattern);
    }
  };

} // namespace Music
