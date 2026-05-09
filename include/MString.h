/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file MString.h
 * @brief Fixed Capacity String
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */

#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <ostream>

template<std::size_t N>
struct MString
{
public:
  /**
   * @brief Default constructor initializes the string to empty.
   * @param s Optional C-style string to initialize the MString with. If not
   * provided, the MString will be initialized to an empty string.
   */
  MString(const char* s = "") { set(s); }

  // constexpr MString(const char (&s)[N]) { set(s); }

  /**
   * @brief Implicit conversion operator to const char* for easy use with
   * C-style strings.
   * @return A pointer to the internal C-style string.
   */
  operator const char*() const { return data.data(); }

  /**
   * @brief Get a pointer to the internal C-style string (null-terminated).
   * @return A pointer to the internal C-style string.
   */
  [[nodiscard]] auto c_str() const -> const char* { return data.data(); }

  /**
   * @brief Get the capacity of the string (maximum number of characters it can
   * hold, excluding the null terminator).
   * @return The capacity of the string.
   */
  [[nodiscard]] constexpr auto capacity() const -> std::size_t { return N; }

  /**
   * @brief Get the length of the string (number of characters before the null
   * terminator).
   * @return The length of the string.
   */
  [[nodiscard]] std::size_t size() const
  {
    std::size_t len = 0;
    while (len < N && data[len] != '\0') {
      ++len;
    }
    return len;
  }

  /**
   * @brief Check if the string is empty (i.e., starts with a null terminator).
   * @return true if the string is empty, false otherwise.
   */
  [[nodiscard]] auto empty() const -> bool { return data[0] == '\0'; }

  /**
   * @brief Clear the string by setting the first character to null terminator.
   */
  void clear() { data.fill('\0'); }

  /**
   * @brief Set the string to a new value, ensuring it does not exceed
   * capacity.
   * @param s The new string value. If nullptr, the string will be cleared.
   * @return true if the string was set successfully, false if it was truncated
   * due to
   */
  auto set(const char* s) -> bool
  {
    if (s == nullptr) {
      clear();
      return true;
    }
    std::size_t i = 0;
    for (; i < N && s[i] != '\0'; ++i) {
      data[i] = s[i];
    }
    data[i] = '\0';
    return s[i] == '\0';
  }

  /**
   * @brief Append a string to the end of the current string, ensuring it does
   * not exceed capacity.
   * @param s The string to append.
   * @return true if the entire string was appended, false if it was truncated
   * due to capacity limits.
   */
  bool append(const char* s)
  {
    if (s == nullptr) {
      return true;
    }
    std::size_t len = size();
    std::size_t i = 0;
    for (; len < N && s[i] != '\0'; ++len, ++i) {
      data[len] = s[i];
    }
    data[len] = '\0';
    return s[i] == '\0';
  }

  /**
   * @brief Format the string using printf-style formatting, ensuring it does
   * not exceed capacity.
   * @param format The printf-style format string.
   * @return The number of characters written, not including the null
   * terminator. If the output was truncated due to capacity limits, the return
   * value is the number of characters that would have been written if enough
   * space had been available.
   */
  int printf(const char* format, ...)
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
  bool operator==(const MString& other) const
  {
    return std::strcmp(data.data(), other.data.data()) == 0;
  }

  /**
   * @brief Compare this MString with another for inequality.
   * @param other The other MString to compare with.
   * @return true if the strings are not equal, false otherwise.
   */
  bool operator!=(const MString& other) const { return !(*this == other); }

  /**
   * @brief Compare this MString with another for ordering (lexicographical
   * comparison).
   * @param other The other MString to compare with.
   * @return true if this MString is less than the other, false otherwise.
   */
  bool operator<(const MString& other) const
  {
    return std::strcmp(data.data(), other.data.data()) < 0;
  }

  /**
   * @brief Compare this MString with another for ordering (lexicographical
   * comparison).
   * @param other The other MString to compare with.
   * @return true if this MString is greater than the other, false otherwise.
   */
  bool operator>(const MString& other) const
  {
    return std::strcmp(data.data(), other.data.data()) > 0;
  }

private:
  std::array<char, N + 1> data; // +1 for null terminator
};

// Inferance & Deduction
// template <std::size_t N> MString(const char (*)[N]) -> MString<N>;

template<std::size_t N>
std::ostream&
operator<<(std::ostream& out, const MString<N>& text)
{
  return out << text.c_str();
}
