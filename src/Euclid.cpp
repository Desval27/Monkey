/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Euclid.cpp
 * @brief Euclidian Rhythm Generator.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @brief
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */

//  #include <stdint.h>
//  #include <algorithm>
#include <Monkey.h>
#include <Music/Music.h>

namespace Music
{
  /////////////////////////////////////////////////////////////////////////////
  /// @brief 
  /// @param k 
  /// @param n 
  /// @param r 
  /// @param pattern 
  /// @return 
  template <std::size_t MAX_EVENTS>
  std::size_t BuildEuclid(int k, int n, int r, PatternEventSet<MAX_EVENTS> &pattern)
  {
    bool buffer[DEF_MAX_EVENTS];
    n = clamp(n, 1, static_cast<int>(std::min(pattern.Capacity(), ArrayLen(buffer))));
    k = clamp(k, 0, n);

    // Start with a clear pattern
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
        uint8_t src = (i + (n - r)) % n; // right rotate
        pattern[i] = buffer[src];
      }
    }
    return pattern.Count();
  }

  template std::size_t BuildEuclid<DEF_MAX_EVENTS>(int k, int n, int r, PatternEventSet<DEF_MAX_EVENTS> &pattern);

} // namespace Music