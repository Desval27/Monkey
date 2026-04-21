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
  // ============================================================
  // -------------------- EUCLIDEAN BUILDER ----------------------
  // Bucket method + rotation
  // ============================================================
  bool _buffer[128];
  size_t BuildEuclid(int k, int n, int r, bool *out, size_t outSize)
  {
    n = clamp(n, 1, static_cast<int>(std::min(outSize, ArrayLen(_buffer))));
    k = clamp(k, 0, n);

    for (uint8_t i = 0; i < n; i++)
      out[i] = false;

    uint16_t bucket = 0;
    for (uint8_t i = 0; i < n; i++)
    {
      bucket += k;
      if (bucket >= n)
      {
        bucket -= n;
        out[i] = true;
      }
    }

    r = (n == 0) ? 0 : (r % n);
    if (r)
    {
      for (uint8_t i = 0; i < n; i++)
        _buffer[i] = out[i];
      for (uint8_t i = 0; i < n; i++)
      {
        uint8_t src = (i + (n - r)) % n; // right rotate
        out[i] = _buffer[src];
      }
    }
    return n;
  }

  size_t BuildEuclid(int k, int n, int r, PatternEventSet<> &pattern)
  {
    n = clamp(n, 1, static_cast<int>(std::min(pattern.Capacity(), ArrayLen(_buffer))));
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
        _buffer[i] = pattern[i];
      for (uint8_t i = 0; i < n; i++)
      {
        uint8_t src = (i + (n - r)) % n; // right rotate
        pattern[i] = _buffer[src];
      }
    }
    return pattern.Count();
  }

} // namespace Music