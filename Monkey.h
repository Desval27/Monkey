/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Monkey.h
 * @brief General Dev Utilities.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#ifndef DAISY_PLATFORM
#include <cstddef>
#include <cstdlib>
#include <cstdint>
#endif


#if USE_DEBUG
#include <cstdarg>
using DebugFunc = void (*)(const char* format, va_list args);
extern void setDebugFunc(DebugFunc f);
extern void dprintf(const char* format, ...);

#define SET_DEBUG(f)        setDebugFunc(f)
#define DEBUG_GOT_HERE()    do { dprintf("%s:%d - %s\n", __FILE__, __LINE__, __func__); } while (0)
#define DPRINTF(...)          do { dprintf(__VA_ARGS__); } while (0)
#else
#define SET_DEBUG(f)
#define DEBUG_GOT_HERE()
#define DPRINTF(...)
#endif

template <typename T>
constexpr const T& clamp(const T& value, const T& low, const T& high)
{
    return (value < low) ? low : (value > high) ? high : value;
}

template <typename T>
constexpr const T wrap(const T value, const T& count)
{
    if(count <= 0)
        return 0;
    const T r = value % count;
    return (r < 0) ? (r + count) : r;
}

template <typename T>
constexpr const T min(const T &valueA, const T &valueB)
{
    return (valueA < valueB) ? valueA : valueB;
}

template <typename T>
constexpr const T max(const T &valueA, const T &valueB)
{
    return (valueA > valueB) ? valueA : valueB;
}

template <typename T>
constexpr const T wrap(const T value, const T& low, const T& high)
{
    return (value < low) ? high : (value > high) ? low : value;
}

template <typename T>
constexpr const T fastPow(T base, int exp)
{
    // Optional: Ensure that T is a floating-point type at compile time
    //static_assert(std::is_floating_point<T>::value, "T must be a floating-point type");

    if(exp == 0)
        return static_cast<T>(1.0);

    T    result = static_cast<T>(1.0);
    bool neg    = (exp < 0);
    int  e      = neg ? -exp : exp;

    while(e > 0)
    {
        if(e & 1)
            result *= base;
        base *= base;
        e >>= 1;
    }

    return neg ? (static_cast<T>(1.0) / result) : result;
}


#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitToggle
#define bitToggle(value, bit) ((value) ^= (1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) \
    ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))
#endif
#ifndef DAISY_PLATFORM
#define rand() std::rand()
#endif

template <typename T>
constexpr const T randomRange(const T& low, const T& high)
{
    return low + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX/(high - low)));
}

template <typename T, size_t N>
constexpr size_t ArrayLen(const T (&)[N])
{
    return N;
}
