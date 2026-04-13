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

#ifndef DAISY_PLATFORM
#include <cmath>
#endif

namespace Music 
{

#if defined(ARDUINO_ARCH_AVR)
#include <avr/pgmspace.h>
#define TUNING_PROGMEM PROGMEM
#define TUNING_PGM_READ_PTR(addr) ((const char *)pgm_read_ptr(addr))
static inline char *
tuning_copy_string(char *dst, const char *src, size_t maxLen)
{
    if(!dst || maxLen == 0)
        return dst;
    if(!src)
    {
        dst[0] = '\0';
        return dst;
    }

    size_t i = 0;
    for(; (i + 1) < maxLen; ++i)
    {
        const char c = (char)pgm_read_byte(src + i);
        dst[i]       = c;
        if(c == '\0')
            return dst;
    }

    dst[maxLen - 1] = '\0';
    return dst;
}
#else
#define TUNING_PROGMEM
#define TUNING_PGM_READ_PTR(addr) (*(addr))
static inline char *
tuning_copy_string(char *dst, const char *src, size_t maxLen)
{
    if(!dst || maxLen == 0)
        return dst;
    if(!src)
    {
        dst[0] = '\0';
        return dst;
    }

    size_t i = 0;
    for(; (i + 1) < maxLen && src[i] != '\0'; ++i)
        dst[i] = src[i];

    dst[i] = '\0';
    return dst;
}
#endif

/**
 * @brief 
 * 
 * @param ratio 
 * @return float 
 */
static inline float ratioToCents(float ratio)
{ return (ratio > 0.0f) ? (1200.0f * logf(ratio) / logf(2.0f)) : 0.0f; }

/**
 * @brief 
 * 
 * @param cents 
 * @return float 
 */
static inline float centsToRatio(float cents)
{ return powf(2.0f, cents / 1200.0f); }

} // namespace Music