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

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <random>
#include <type_traits>

#ifndef DAISY_PLATFORM
#include <ostream>
#endif

#ifdef USE_DEBUG
#include <cstdarg>
using DebugFunc = void (*)(const char *format, va_list args);
extern void setDebugFunc(DebugFunc f);
extern void dprintf(const char *format, ...);

#define SET_DEBUG(f) setDebugFunc(f)
#define DEBUG_GOT_HERE()                                       \
    do                                                         \
    {                                                          \
        dprintf("%s:%d - %s\n", __FILE__, __LINE__, __func__); \
    } while (0)
#define DPRINTF(...)          \
    do                        \
    {                         \
        dprintf(__VA_ARGS__); \
    } while (0)
#else
#define SET_DEBUG(f)
#define DEBUG_GOT_HERE()
#define DPRINTF(...)
#endif

template <std::size_t N>
struct MString
{
public:
    /**
     * @brief Default constructor initializes the string to empty.
     * @param s Optional C-style string to initialize the MString with. If not provided, the MString will be initialized to an empty string.
     */
    MString(const char *s = "")
    {
        set(s);
    }

    /**
     * @brief Implicit conversion operator to const char* for easy use with C-style strings.
     * @return A pointer to the internal C-style string.
     */
    operator const char *() const
    {
        return data.data();
    }

    /**
     * @brief Get a pointer to the internal C-style string (null-terminated).
     * @return A pointer to the internal C-style string.
     */
    const char* c_str() const
    {
        return data.data();
    }

    /**
     * @brief Get the capacity of the string (maximum number of characters it can hold, excluding the null terminator).
     * @return The capacity of the string.
     */
    constexpr std::size_t capacity() const
    {
        return N;
    }

    /**
     * @brief Get the length of the string (number of characters before the null terminator).
     * @return The length of the string.
     */
    std::size_t size() const
    {
        std::size_t len = 0;
        while (len < N && data[len] != '\0')
            ++len;
        return len;
    }

    /**
     * @brief Check if the string is empty (i.e., starts with a null terminator).
     * @return true if the string is empty, false otherwise.
     */
    bool empty() const
    {
        return data[0] == '\0';
    }

    /**
     * @brief Clear the string by setting the first character to null terminator.
     */
    void clear()
    {
        data.fill('\0');
    }

    /**
     * @brief Set the string to a new value, ensuring it does not exceed capacity.
     * @param s The new string value. If nullptr, the string will be cleared.
     * @return true if the string was set successfully, false if it was truncated due to
     */
    bool set(const char *s)
    {
        if (!s)
        {
            clear();
            return true;
        }
        std::size_t i = 0;
        for (; i < N && s[i] != '\0'; ++i)
            data[i] = s[i];
        data[i] = '\0';
        return s[i] == '\0';
    }

    /**
     * @brief Append a string to the end of the current string, ensuring it does not exceed capacity.
     * @param s The string to append.   
     * @return true if the entire string was appended, false if it was truncated due to capacity limits.
     */
    bool append(const char *s)
    {
        if (!s)
            return true;
        std::size_t len = size();
        std::size_t i = 0;
        for (; len < N && s[i] != '\0'; ++len, ++i)
            data[len] = s[i];
        data[len] = '\0';
        return s[i] == '\0';
    }

    /**
     * @brief Format the string using printf-style formatting, ensuring it does not exceed capacity.
     * @param format The printf-style format string.    
     * @return The number of characters written, not including the null terminator. If the output was truncated due to capacity limits, the return value is the number of characters that would have been written if enough space had been available.
     */
    int printf(const char *format, ...)
    {
        va_list args;
        va_start(args, format);
        const int written = std::vsnprintf(data.data(), N + 1, format, args);
        va_end(args);
        return written;
    }

    /**
     * @brief Compare this MString with another for equality.
     * @param other The other MString to compare with.  
     * @return true if the strings are equal, false otherwise.
     */
    bool operator==(const MString &other) const
    {
        return std::strcmp(data.data(), other.data.data()) == 0;
    } 

    /**
     * @brief Compare this MString with another for inequality.
     * @param other The other MString to compare with.
     * @return true if the strings are not equal, false otherwise.
     */
    bool operator!=(const MString &other) const
    {
        return !(*this == other);
    }   

    /**
     * @brief Compare this MString with another for ordering (lexicographical comparison).
     * @param other The other MString to compare with.  
     * @return true if this MString is less than the other, false otherwise.
     */
    bool operator<(const MString &other) const
    {
        return std::strcmp(data.data(), other.data.data()) < 0;
    }   

    /**
     * @brief Compare this MString with another for ordering (lexicographical comparison).
     * @param other The other MString to compare with.
     * @return true if this MString is greater than the other, false otherwise.
     */
    bool operator>(const MString &other) const
    {
        return std::strcmp(data.data(), other.data.data()) > 0;
    }       

private:
    std::array<char, N + 1> data; // +1 for null terminator
};

#ifndef DAISY_PLATFORM
template <std::size_t N>
std::ostream &operator<<(std::ostream &out, const MString<N> &text)
{
    return out << text.c_str();
}
#endif

/**
 * @brief Get the minimum of two values.
 * @param valueA The first value.
 * @param valueB The second value.
 * @return The minimum of the two values.
 */
template <typename T>
constexpr const T min(const T &valueA, const T &valueB)
{
    return (valueA < valueB) ? valueA : valueB;
}

/**
 * @brief Get the maximum of two values.
 * @param valueA The first value.
 * @param valueB The second value.
 * @return The maximum of the two values.
 */
template <typename T>
constexpr const T max(const T &valueA, const T &valueB)
{
    return (valueA > valueB) ? valueA : valueB;
}

/**
 * @brief Clamp a value to a specified range. If the value is less than the low bound, the low bound is returned. If the value is greater than the high bound, the high bound is returned. Otherwise, the value itself is returned.
 * @param value The value to clamp.
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return The clamped value.
 */
template <typename T>
constexpr const T &clamp(const T &value, const T &low, const T &high)
{
    return (value < low) ? low : (value > high) ? high
                                                : value;
}

/**
 * @brief Wrap a value around a specified count. If the value is negative, it wraps around from the end. If the value is greater than or equal to the count, it wraps around from the beginning.
 * @param value The value to wrap.  
 * @param count The count to wrap around. Must be greater than 0.
 * @return The wrapped value.
 * @note If count is less than or equal to 0, the function returns 0 to avoid division by zero.
 */
template <typename T>
constexpr const T wrap(const T value, const T &count)
{
    if (count <= 0)
        return 0;
    const T r = value % count;
    return (r < 0) ? (r + count) : r;
}

/**
 * @brief Wrap a value to a specified range. If the value is less than the low bound, it wraps around from the high bound. If the value is greater than the high bound, it wraps around from the low bound. Otherwise, the value itself is returned.
 * @param value The value to wrap.  
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return The wrapped value.
 * @note If low is greater than high, the function returns low to avoid an infinite loop.
 */
template <typename T>
constexpr const T wrap(const T value, const T &low, const T &high)
{
    return (value < low) ? high : (value > high) ? low
                                                 : value;
}

/**
 * @brief Fast exponentiation by squaring algorithm to compute base raised to the power of exp. This method is efficient and works in logarithmic time complexity.
 * @param base The base value to be raised to a power.
 * @param exp The exponent value. Can be positive, negative, or zero. If negative, the function returns the reciprocal of the result for the positive exponent.
 * @return The result of base raised to the power of exp.
 * @note If exp is zero, the function returns 1. If exp is negative, the function returns 1 divided by the result of base raised to the positive exponent. If base is zero and exp is negative, the behavior is undefined (division by zero).
 */
template <typename T>
constexpr const T fastPow(T base, int exp)
{
    // Optional: Ensure that T is a floating-point type at compile time
    // static_assert(std::is_floating_point<T>::value, "T must be a floating-point type");

    if (exp == 0)
        return static_cast<T>(1.0);

    T result = static_cast<T>(1.0);
    bool neg = (exp < 0);
    int e = neg ? -exp : exp;

    while (e > 0)
    {
        if (e & 1)
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
// #define rand() std::rand()
#endif

/**
 * @brief A simple struct to represent a range of values with a low and high bound.
 * @tparam T The type of the values in the range. Must support comparison operators.    
 */
template <typename T>
struct Range
{
    Range() : low(0), high(0) {};
    Range(T low, T high) : low(low), high(high) {};

    T low;      ///< The lower bound of the range.
    T high;     ///< The upper bound of the range.
};

/**
 * @brief A struct to represent a range of values with a default value. The default value can be used as a fallback or initial value when working with the range.
 * @tparam T The type of the values in the range. Must support comparison operators.
 */
template <typename T>
struct RangeWithDefault
{
    RangeWithDefault() : low(0), high(0), def(0) {};
    RangeWithDefault(T low, T high, T def) : low(low), high(high), def(def) {};

    T low;      ///< The lower bound of the range.
    T high;     ///< The upper bound of the range.
    T def;      ///< The default value for the range.
};

#ifdef DAISY_PLATFORM
template <typename T>
constexpr const T randomRange(const T &low, const T &high)
{
    return low + static_cast<T>(rand()) / (static_cast<T>(RAND_MAX / (high - low)));
}
#else

/**
 * @brief Generate a random number within a specified range. The function uses the C++11 <random> library to provide better randomness and thread safety compared to the C-style rand() function.
 * @param low The lower bound of the range.
 * @param high The upper bound of the range.
 * @return A random number of type T between low and high (inclusive). If low is greater than high, the function will return a value in the range [high, low].
 */
template <typename T>
T randomRange(T low, T high)
{
    static thread_local std::mt19937 gen{std::random_device{}()};

    if constexpr (std::is_integral_v<T>)
    {
        std::uniform_int_distribution<T> dist(low, high);
        return dist(gen);
    }
    else
    {
        std::uniform_real_distribution<T> dist(low, high);
        return dist(gen);
    }
}
#endif

/**
 * @brief Generate a random number within a specified range defined by a Range struct. This is a convenience overload that allows you to pass a Range object directly instead of separate low and high values.
 * @param range A Range object that defines the low and high bounds for the random number generation.
 * @return A random number of type T between range.low and range.high (inclusive). If
 */
template <typename T>
T randomRange(const Range<T> &range)
{
    return randomRange(range.low, range.high);
}

/**
 * @brief Get the length of a C-style array at compile time. This function template takes a reference to an array and returns its size as a constexpr value. It is a safer alternative to using sizeof(array) / sizeof(array[0]) for determining the length of an array, as it does not decay to a pointer and can be used in compile-time contexts.
 * @param arr A reference to a C-style array of any type and size.  
 */
template <typename T, std::size_t N>
constexpr std::size_t ArrayLen(const T (&)[N])
{
    return N;
}
