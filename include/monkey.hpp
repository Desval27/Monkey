/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file monkey.hpp
 * @brief General Dev Utilities.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <array>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <random>
#include <type_traits>

// #include <mstring.hpp>

#ifdef USE_DEBUG
using DebugFunc = void (*)(const char* format, va_list args);
extern void
set_debug_func(DebugFunc f);
extern void
dprintf(const char* format, ...);

#define SET_DEBUG(f) set_debug_func(f)
#define DEBUG_GOT_HERE()                                                       \
  do {                                                                         \
    dprintf("%s:%d - %s\n", __FILE__, __LINE__, __func__);                     \
  } while (0)
#define DPRINTF(...)                                                           \
  do {                                                                         \
    dprintf(__VA_ARGS__);                                                      \
  } while (0)
#else
#define SET_DEBUG(f)
#define DEBUG_GOT_HERE()
#define DPRINTF(...)
#endif

/**
 * @brief get the minimum of two values.
 * @param valueA The first value.
 * @param valueB The second value.
 * @return The minimum of the two values.
 */
template<typename TEventType>
constexpr const TEventType
min(const TEventType& valueA, const TEventType& valueB)
{
  return (valueA < valueB) ? valueA : valueB;
}

/**
 * @brief get the maximum of two values.
 * @param valueA The first value.
 * @param valueB The second value.
 * @return The maximum of the two values.
 */
template<typename TEventType>
constexpr TEventType
max(const TEventType& valueA, const TEventType& valueB)
{
  return (valueA > valueB) ? valueA : valueB;
}

/**
 * @brief Clamp a value to a specified range. If the value is less than the low
 * bound, the low bound is returned. If the value is greater than the high
 * bound, the high bound is returned. Otherwise, the value itself is returned.
 * @param value The value to clamp.
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return The clamped value.
 */
template<typename TEventType>
constexpr TEventType
clamp(const TEventType& value, const TEventType& low, const TEventType& high)
{
  return (value < low) ? low : (value > high) ? high : value;
}

/**
 * @brief Wrap a value around a specified count. If the value is negative, it
 * wraps around from the end. If the value is greater than or equal to the
 * count, it wraps around from the beginning.
 * @param value The value to wrap.
 * @param count The count to wrap around. Must be greater than 0.
 * @return The wrapped value.
 * @note If count is less than or equal to 0, the function returns 0 to avoid
 * division by zero.
 */
template<typename TEventType>
constexpr TEventType
wrap(const TEventType value, const TEventType& count)
{
  if (count <= 0) {
    return 0;
  }
  const TEventType r = value % count;
  return (r < 0) ? (r + count) : r;
}

/**
 * @brief Wrap a value to a specified range. If the value is less than the low
 * bound, it wraps around from the high bound. If the value is greater than the
 * high bound, it wraps around from the low bound. Otherwise, the value itself
 * is returned.
 * @param value The value to wrap.
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return The wrapped value.
 * @note If low is greater than high, the function returns low to avoid an
 * infinite loop.
 */
template<typename TEventType>
constexpr const TEventType
wrap(const TEventType value, const TEventType& low, const TEventType& high)
{
  return (value < low) ? high : (value > high) ? low : value;
}

/**
 * @brief Fast exponentiation by squaring algorithm to compute base raised to
 * the power of exp. This method is efficient and works in logarithmic time
 * complexity.
 * @param base The base value to be raised to a power.
 * @param exp The exponent value. Can be positive, negative, or zero. If
 * negative, the function returns the reciprocal of the result for the positive
 * exponent.
 * @return The result of base raised to the power of exp.
 * @note If exp is zero, the function returns 1. If exp is negative, the
 * function returns 1 divided by the result of base raised to the positive
 * exponent. If base is zero and exp is negative, the behavior is undefined
 * (division by zero).
 */
template<typename TEventType>
constexpr const TEventType
fast_pow(TEventType base, int exp)
{
  // Optional: Ensure that T is a floating-point type at compile time
  // static_assert(std::is_floating_point<T>::value, "T must be a
  // floating-point type");

  if (exp == 0) {
    return static_cast<TEventType>(1.0);
  }

  TEventType result = static_cast<TEventType>(1.0);
  bool neg = (exp < 0);
  int e = neg ? -exp : exp;

  while (e > 0) {
    if (e & 1) {
      result *= base;
    }
    base *= base;
    e >>= 1;
  }

  return neg ? (static_cast<TEventType>(1.0) / result) : result;
}

#ifndef bit_read
#define bit_read(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bit_set
#define bit_set(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bit_clear
#define bit_clear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bit_toggle
#define bit_toggle(value, bit) ((value) ^= (1UL << (bit)))
#endif
#ifndef bit_write
#define bit_write(value, bit, bit_value)                                       \
  ((bit_value) ? bitSet(value, bit) : bitClear(value, bit))
#endif

/**
 * @brief A simple struct to represent a range of values with a low and high
 * bound.
 * @tparam T The type of the values in the range. Must support comparison
 * operators.
 */
template<typename TEventType>
struct Range
{
  Range()
    : low(0)
    , high(0) {};
  Range(TEventType low, TEventType high)
    : low(low)
    , high(high) {};

  TEventType low;  ///< The lower bound of the range.
  TEventType high; ///< The upper bound of the range.
};

/**
 * @brief A struct to represent a range of values with a default value. The
 * default value can be used as a fallback or initial value when working with
 * the range.
 * @tparam T The type of the values in the range. Must support comparison
 * operators.
 */
template<typename TEventType>
struct RangeWithDefault
{
  RangeWithDefault()
    : low(0)
    , high(0)
    , def(0) {};
  RangeWithDefault(TEventType low, TEventType high, TEventType def)
    : low(low)
    , high(high)
    , def(def) {};

  TEventType low;  ///< The lower bound of the range.
  TEventType high; ///< The upper bound of the range.
  TEventType def;  ///< The default value for the range.
};

#ifdef DAISY_PLATFORM
template<typename T>
constexpr const T
random_range(const T& low, const T& high)
{
  return low +
         static_cast<T>(rand()) / (static_cast<T>(RAND_MAX / (high - low)));
}
#else

/**
 * @brief Generate a random number within a specified range. The function uses
 * the C++11 <random> library to provide better randomness and thread safety
 * compared to the C-style rand() function.
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return A random number of type T between low and high (inclusive). If low
 * is greater than high, the function will return a value in the range [high,
 * low].
 */
template<typename TEventType>
TEventType
random_range(TEventType low, TEventType high)
{
  static thread_local std::mt19937 gen{ std::random_device{}() };

  if constexpr (std::is_integral_v<TEventType>) {
    std::uniform_int_distribution<TEventType> dist(low, high);
    return dist(gen);
  } else {
    std::uniform_real_distribution<TEventType> dist(low, high);
    return dist(gen);
  }
}
#endif

/**
 * @brief Generate a random number within a specified range defined by a Range
 * struct. This is a convenience overload that allows you to pass a Range
 * object directly instead of separate low and high values.
 * @param range A Range object that defines the low and high bounds for the
 * random number generation.
 * @return A random number of type T between range.low and range.high
 * (inclusive). If
 */
template<typename TEventType>
TEventType
random_range(const Range<TEventType>& range)
{
  return random_range(range.low, range.high);
}

/**
 * @brief get the length of a C-style array at compile time. This function
 * template takes a reference to an array and returns its size as a constexpr
 * value. It is a safer alternative to using sizeof(array) / sizeof(array[0])
 * for determining the length of an array, as it does not decay to a pointer
 * and can be used in compile-time contexts.
 * @param arr A reference to a C-style array of any type and size.
 */
template<typename TEventType, std::size_t N>
constexpr std::size_t
array_len(const TEventType (& /*unused*/)[N])
{
  return N;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam T
/// @tparam N
/// @param values
/// @param count
/// @return
template<typename T, std::size_t N>
T
min_value(const std::array<T, N>& values, std::size_t count)
{
  count = std::min(count, values.size());
  if (count == 0) {
    return 0;
  }

  T best = values[0];
  for (std::size_t i = 1; i < count; ++i) {
    best = std::min(values[i], best);
  }
  return best;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam T
/// @tparam N
/// @param values
/// @param count
/// @return
template<typename T, std::size_t N>
T
max_value(const std::array<T, N>& values, std::size_t count)
{
  count = std::min(count, values.size());
  if (count == 0) {
    return 0;
  }

  T best = values[0];
  for (std::size_t i = 1; i < count; ++i) {
    best = std::max(values[i], best);
  }
  return best;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam T
/// @tparam N
/// @param values
/// @param count
/// @param limit
/// @return
template<typename T, std::size_t N>
T
greatest_value_less_than(const std::array<T, N>& values,
                         std::size_t count,
                         T limit)
{
  count = std::min(count, values.size());
  T best = 0;
  bool found = false;
  for (std::size_t i = 0; i < count; ++i) {
    if (values[i] < limit && (!found || values[i] > best)) {
      best = values[i];
      found = true;
    }
  }
  return found ? best : limit;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam T
/// @tparam N
/// @param values
/// @param count
/// @param limit
/// @return
template<typename T, std::size_t N>
T
least_value_greater_than(const std::array<T, N>& values,
                         std::size_t count,
                         T limit)
{
  count = std::min(count, values.size());
  T best = 0;
  bool found = false;
  for (std::size_t i = 0; i < count; ++i) {
    if (values[i] > limit && (!found || values[i] < best)) {
      best = values[i];
      found = true;
    }
  }
  return found ? best : limit;
}
