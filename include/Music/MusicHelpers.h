/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file MusicHelpers.h
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <cstddef>

#ifndef DAISY_PLATFORM
#include <cmath>
#endif

namespace Music {
#ifdef ARDUINO_ARCH_AVR
#include <avr/pgmspace.h>
#define TUNING_PROGMEM PROGMEM
#define TUNING_PGM_READ_PTR(addr) ((const char*)pgm_read_ptr(addr))
static inline char*
tuning_copy_string(char* dst, const char* src, std::size_t maxLen)
{
  if (!dst || maxLen == 0)
    return dst;
  if (!src) {
    dst[0] = '\0';
    return dst;
  }

  std::size_t i = 0;
  for (; (i + 1) < maxLen; ++i) {
    const char c = (char)pgm_read_byte(src + i);
    dst[i] = c;
    if (c == '\0')
      return dst;
  }

  dst[maxLen - 1] = '\0';
  return dst;
}
#else
#define TUNING_PROGMEM
#define TUNING_PGM_READ_PTR(addr) (*(addr))
static inline char*
tuning_copy_string(char* dst, const char* src, std::size_t maxLen)
{
  if ((dst == nullptr) || maxLen == 0) {
    return dst;
  }
  if (src == nullptr) {
    dst[0] = '\0';
    return dst;
  }

  std::size_t i = 0;
  for (; (i + 1) < maxLen && src[i] != '\0'; ++i) {
    dst[i] = src[i];
  }

  dst[i] = '\0';
  return dst;
}
#endif

/**
 * @brief Converts a frequency ratio to cents, where 1200 cents equals an
 * octave. This is a common calculation in music theory for expressing
 * intervals in a logarithmic scale. The function takes a positive frequency
 * ratio as input and returns the corresponding number of cents, calculated
 * using the formula: cents = 1200 * log2(ratio). If the input ratio is zero or
 * negative, the function returns 0.0f to avoid invalid logarithm calculations.
 * @param ratio The frequency ratio to convert, where a ratio of 1.0
 * corresponds to unison, 2.0 corresponds to an octave, and values between 1.0
 * and 2.0 correspond to intervals within the octave.
 * @return float The number of cents corresponding to the given frequency
 * ratio, calculated as 1200 times the base-2 logarithm of the ratio.
 */
static inline float
RatioToCents(float ratio)
{
  return (ratio > 0.0F) ? (1200.0F * logf(ratio) / logf(2.0F)) : 0.0F;
}

/**
 * @brief Converts a cent value to a frequency ratio.
 * @param cents The number of cents to convert, where 1200 cents equals an
 * octave.
 * @return float The frequency ratio corresponding to the given cent value,
 * calculated as 2 raised to the power of (cents divided by 1200).
 */
static inline float
CentsToRatio(float cents)
{
  return powf(2.0F, cents / 1200.0F);
}

/**
 * @brief Converts a BPM value to a frequency in Hz, given a specific PPQN
 * (Pulses Per Quarter Note) resolution. This is useful for timing calculations
 * in music applications, where the tempo is often expressed in BPM and the
 * timing of events is based on a certain number of pulses per quarter note.
 * @param bpm The tempo in beats per minute.
 * @param ppqn The number of pulses per quarter note, which defines the timing
 * resolution
 * @return float The frequency in Hz corresponding to the given BPM and PPQN.
 */
static inline float
BpmToFreq(float bpm, int ppqn)
{
  return (bpm * static_cast<float>(ppqn)) / 60.0F;
}

/**
 * @brief Converts a BPM value to a frequency in Hz using a default PPQN
 * (Pulses Per Quarter Note) resolution defined by the constant PPQN. This is a
 * convenience overload of the BpmToFreq function for cases where the default
 * PPQN is sufficient for timing calculations.
 * @param bpm The tempo in beats per minute.
 * @return float The frequency in Hz corresponding to the given BPM and the
 * default PPQN
 */
static inline float
BpmToFreq(float bpm)
{
  return BpmToFreq(bpm, 1);
}

} // namespace Music
