/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file pattern_generator.hpp
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#ifdef USE_DEBUG
#include <iostream>
#include <tty.h>
#endif

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include <monkey.hpp>
#include <music/music.hpp>
#include <music/music_templates.hpp>

namespace music {

template<std::size_t MAX_EVENTS>
std::size_t
build_euclid(int k, int n, int r, PatternEventSet<MAX_EVENTS>& pattern)
{
  bool buffer[DEF_MAX_EVENTS];
  n = clamp(
    n, 1, static_cast<int>(std::min(pattern.capacity(), ArrayLen(buffer))));
  k = clamp(k, 0, n);

  for (int i = 0; i < n; i++) {
    pattern.add(false);
  }

  uint16_t bucket = 0;
  for (int i = 0; i < n; i++) {
    bucket += k;
    if (bucket >= n) {
      bucket -= n;
      pattern[i] = true;
    }
  }

  r = (n == 0) ? 0 : (r % n);
  if (r) {
    for (int i = 0; i < n; i++) {
      buffer[i] = pattern[i];
    }
    for (int i = 0; i < n; i++) {
      uint8_t src = (i + (n - r)) % n;
      pattern[i] = buffer[src];
    }
  }
  return pattern.size();
}

//
// Different Pattern Generators
//

/**
 * @brief A pattern generator that produces no events. Useful for testing
 * and as a default generator.
 */
template<std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class NullPatternGenerator
{
public:
  static std::size_t generate_pattern(const TimeSignature& ts,
                                     int bars,
                                     float density,
                                     NoteValue granularity,
                                     PatternEventSet<MAX_EVENTS>& pattern)
  {
    pattern.clear();
    return pattern.size();
  }
};

/**
 * @brief A pattern generator that produces random events based on a
 * specified density. Each potential event slot is filled with an event
 * with a probability equal to the density parameter.
 *
 * The generate_pattern function calculates the total number of event slots
 * based on the time signature, number of bars, and granularity, and then
 * randomly fills those slots according to the specified density. This
 * generator can produce patterns with varying levels of activity, from
 * sparse to dense, depending on the density value provided.
 */
template<std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class SimpleRandomPatternGenerator
{
public:
  static std::size_t generate_pattern(const TimeSignature& ts,
                                     int bars,
                                     float density,
                                     NoteValue granularity,
                                     PatternEventSet<MAX_EVENTS>& pattern)
  {
    if (pattern.capacity() == 0 || bars == 0 ||
        ts.beatValue == NoteValue::None || granularity == NoteValue::None) {
      return 0; // Sanity check
    }

    std::size_t t = static_cast<std::size_t>(bars) * ts.beats * ts.beatValue;
    std::size_t n = t / granularity;
    std::size_t r = t % granularity;
#ifdef USE_DEBUG
    std::clog << TTY_FG_RED << "Simple Pattern:" << TTY_RESET << " " << t << "/"
              << static_cast<unsigned int>(granularity)
              << " slots/granularity = " << n << " events with " << r
              << " slots remaining" << '\n';
#endif

    std::size_t eventsToEmit = min(n, pattern.capacity());
    for (std::size_t i = 0; i < eventsToEmit; i++) {
      pattern.emplace(randomRange(0.0F, 1.0F) < density);
    }
    return pattern.size();
  }
};

/**
 * @brief
 */
template<std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class InversionPatternGenerator
{
public:
  static std::size_t generate_pattern(const TimeSignature& ts,
                                     int bars,
                                     float density,
                                     NoteValue granularity,
                                     PatternEventSet<MAX_EVENTS>& pattern)
  {
    if (pattern.capacity() == 0 || bars == 0 ||
        ts.beatValue == NoteValue::None || granularity == NoteValue::None ||
        pattern.size() == 0) {
      return 0; // Sanity check
    }

    std::size_t t = static_cast<std::size_t>(bars) * ts.beats * ts.beatValue;
    std::size_t n = t / granularity;
    std::size_t r = t % granularity;
#ifdef USE_DEBUG
    std::clog << TTY_FG_RED << TTY_INVERSE_ON
              << "Inversion Pattern:" << TTY_RESET << " " << t << "/"
              << static_cast<unsigned int>(granularity)
              << " slots/granularity = " << n << " events with " << r
              << " slots remaining" << '\n';
#endif

    // In case the bars/beats had changed from one version to the other.
    std::size_t eventsToEmit = min(n, pattern.size());
    for (std::size_t i = 0; i < eventsToEmit; i++) {
      pattern[i] = !pattern[i];
    }
    return pattern.size();
  }
};

/**
 * @brief A pattern generator that produces Euclidean rhythms based on the
 * specified density. The generator calculates the number of events to emit
 * based on the density and then uses the Euclidean algorithm to distribute
 * those events as evenly as possible across the available slots defined by
 * the time signature and granularity.
 */
template<std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class EuclidianPatternGenerator
{
public:
  static std::size_t generate_pattern(const TimeSignature& ts,
                                     int bars,
                                     float density,
                                     NoteValue granularity,
                                     PatternEventSet<MAX_EVENTS>& pattern)
  {
    if (pattern.capacity() == 0 || bars == 0 ||
        ts.beatValue == NoteValue::None || granularity == NoteValue::None) {
      return 0; // Sanity check
    }

    int t = bars * ts.beats * ts.beatValue;
    int n = t / granularity;
    int r = t % granularity;
    double k1 = static_cast<float>(n) * density;
    int k = static_cast<int>(trunc(k1));
#ifdef USE_DEBUG
    std::clog << TTY_FG_RED << "Eulclidian Pattern:" << TTY_RESET << " " << t
              << "/" << static_cast<unsigned int>(granularity)
              << " slots/granularity = " << k << " events out of " << n
              << " with " << r << " slots remaining" << '\n';
#endif

    return build_euclid(k, n, 1, pattern);
  }
};

template<std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class RandomRandomPatternGenerator
{
public:
  static std::size_t generate_pattern(const TimeSignature& ts,
                                     int bars,
                                     float density,
                                     NoteValue granularity,
                                     PatternEventSet<MAX_EVENTS>& pattern)
  {
    switch (randomRange(0, 2)) {
      case 0:
        pattern.clear();
        SimpleRandomPatternGenerator<MAX_EVENTS>::generate_pattern(
          ts, bars, density, granularity, pattern);
        break;
      case 1:
        pattern.clear();
        EuclidianPatternGenerator<MAX_EVENTS>::generate_pattern(
          ts, bars, density, granularity, pattern);
        break;
      case 2:
        if (pattern.size() > 0) {
          InversionPatternGenerator<MAX_EVENTS>::generate_pattern(
            ts, bars, density, granularity, pattern);
        } else {
          // Fallback
          pattern.clear();
          SimpleRandomPatternGenerator<MAX_EVENTS>::generate_pattern(
            ts, bars, density, granularity, pattern);
        }
        break;
      default:
        pattern.clear();
        break;
    }
#ifdef USE_DEBUG
    DebugPattern<MAX_EVENTS>(ts, granularity, pattern);
    std::cout << '\n';
#endif
    return pattern.size();
  }
};

} // namespace Music
