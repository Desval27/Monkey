#pragma once

#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Test {

struct Failure : public std::exception
{
  explicit Failure(std::string message)
    : _message(std::move(message))
  {
  }

  const char* what() const noexcept override { return _message.c_str(); }

private:
  std::string _message;
};

struct TestCase
{
  const char* name;
  void (*fn)();
};

inline std::vector<TestCase>&
Registry()
{
  static std::vector<TestCase> tests;
  return tests;
}

inline bool
Register(const char* name, void (*fn)())
{
  Registry().push_back({ name, fn });
  return true;
}

inline bool
AreEqual(const char* actual, const char* expected)
{
  if (actual == nullptr || expected == nullptr) {
    return actual == expected;
  }
  return std::strcmp(actual, expected) == 0;
}

inline bool
AreEqual(char* actual, char* expected)
{
  return AreEqual(static_cast<const char*>(actual),
                  static_cast<const char*>(expected));
}

inline bool
AreEqual(char* actual, const char* expected)
{
  return AreEqual(static_cast<const char*>(actual), expected);
}

inline bool
AreEqual(const char* actual, char* expected)
{
  return AreEqual(actual, static_cast<const char*>(expected));
}

template<typename A, typename B>
inline bool
AreEqual(const A& actual, const B& expected)
{
  return actual == expected;
}

template<typename A, typename B>
inline void
CheckEq(const A& actual,
        const B& expected,
        const char* actualExpr,
        const char* expectedExpr,
        const char* file,
        int line)
{
  if (!AreEqual(actual, expected)) {
    std::ostringstream out;
    out << file << ':' << line << " CHECK_EQ failed: " << actualExpr
        << " == " << expectedExpr << " (actual=" << actual
        << ", expected=" << expected << ')';
    throw Failure(out.str());
  }
}

template<typename A, typename B>
inline void
CheckNe(const A& actual,
        const B& expected,
        const char* actualExpr,
        const char* expectedExpr,
        const char* file,
        int line)
{
  if (AreEqual(actual, expected)) {
    std::ostringstream out;
    out << file << ':' << line << " CHECK_NE failed: " << actualExpr
        << " != " << expectedExpr << " (actual=" << actual
        << ", expected=" << expected << ')';
    throw Failure(out.str());
  }
}

template<typename A, typename B>
inline void
CheckLt(const A& actual,
        const B& expected,
        const char* actualExpr,
        const char* expectedExpr,
        const char* file,
        int line)
{
  if (!(actual < expected)) {
    std::ostringstream out;
    out << file << ':' << line << " CHECK_LT failed: " << actualExpr << " < "
        << expectedExpr << " (actual=" << actual << ", expected=" << expected
        << ')';
    throw Failure(out.str());
  }
}

template<typename A, typename B>
inline void
CheckGt(const A& actual,
        const B& expected,
        const char* actualExpr,
        const char* expectedExpr,
        const char* file,
        int line)
{
  if (!(actual > expected)) {
    std::ostringstream out;
    out << file << ':' << line << " CHECK_GT failed: " << actualExpr << " > "
        << expectedExpr << " (actual=" << actual << ", expected=" << expected
        << ')';
    throw Failure(out.str());
  }
}

template<typename A, typename B>
inline void
CheckLe(const A& actual,
        const B& expected,
        const char* actualExpr,
        const char* expectedExpr,
        const char* file,
        int line)
{
  if (!(actual <= expected)) {
    std::ostringstream out;
    out << file << ':' << line << " CHECK_LE failed: " << actualExpr
        << " <= " << expectedExpr << " (actual=" << actual
        << ", expected=" << expected << ')';
    throw Failure(out.str());
  }
}

template<typename A, typename B>
inline void
CheckGe(const A& actual,
        const B& expected,
        const char* actualExpr,
        const char* expectedExpr,
        const char* file,
        int line)
{
  if (!(actual >= expected)) {
    std::ostringstream out;
    out << file << ':' << line << " CHECK_GE failed: " << actualExpr
        << " >= " << expectedExpr << " (actual=" << actual
        << ", expected=" << expected << ')';
    throw Failure(out.str());
  }
}

template<typename T, typename L, typename U>
inline void
CheckInRange(const T& actual,
             const L& lower,
             const U& upper,
             const char* actualExpr,
             const char* lowerExpr,
             const char* upperExpr,
             const char* macroName,
             const char* file,
             int line)
{
  if (!(actual >= lower && actual <= upper)) {
    std::ostringstream out;
    out << file << ':' << line << ' ' << macroName << " failed: " << actualExpr
        << " in [" << lowerExpr << ", " << upperExpr << "] (actual=" << actual
        << ", lower=" << lower << ", upper=" << upper << ')';
    throw Failure(out.str());
  }
}

inline void
Check(bool condition, const char* expr, const char* file, int line)
{
  if (!condition) {
    std::ostringstream out;
    out << file << ':' << line << " CHECK failed: " << expr;
    throw Failure(out.str());
  }
}

inline int
RunAll()
{
  int failed = 0;
  int passed = 0;

  for (const auto& test : Registry()) {
    try {
      test.fn();
      ++passed;
      std::cout << "[PASS] " << test.name << '\n';
    } catch (const Failure& failure) {
      ++failed;
      std::cerr << "[FAIL] " << test.name << ": " << failure.what() << '\n';
    } catch (const std::exception& exception) {
      ++failed;
      std::cerr << "[FAIL] " << test.name
                << ": unexpected exception: " << exception.what() << '\n';
    } catch (...) {
      ++failed;
      std::cerr << "[FAIL] " << test.name << ": unknown exception\n";
    }
  }

  std::cout << "Summary: " << passed << " passed, " << failed << " failed\n\n";
  return failed == 0 ? 0 : 1;
}

} // namespace Test

#define TEST_CONCAT_IMPL(a, b) a##b
#define TEST_CONCAT(a, b) TEST_CONCAT_IMPL(a, b)

#define TEST_CASE(name)                                                        \
  static void TEST_CONCAT(TestCaseFn_, __LINE__)();                            \
  static const bool TEST_CONCAT(TestCaseReg_, __LINE__) =                      \
    Test::Register(name, &TEST_CONCAT(TestCaseFn_, __LINE__));                 \
  static void TEST_CONCAT(TestCaseFn_, __LINE__)()

#define CHECK(expr) Test::Check((expr), #expr, __FILE__, __LINE__)
#define CHECK_EQ(actual, expected)                                             \
  Test::CheckEq((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define CHECK_NE(actual, expected)                                             \
  Test::CheckNe((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define CHECK_LT(actual, expected)                                             \
  Test::CheckLt((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define CHECK_GT(actual, expected)                                             \
  Test::CheckGt((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define CHECK_LE(actual, expected)                                             \
  Test::CheckLe((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define CHECK_GE(actual, expected)                                             \
  Test::CheckGe((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define CHECK_CONTAINS(actual, lower, upper)                                   \
  Test::CheckInRange((actual),                                                 \
                     (lower),                                                  \
                     (upper),                                                  \
                     #actual,                                                  \
                     #lower,                                                   \
                     #upper,                                                   \
                     "CHECK_CONTAINS",                                         \
                     __FILE__,                                                 \
                     __LINE__)
#define CHECK_IN_BOUNDS(actual, lower, upper)                                  \
  Test::CheckInRange((actual),                                                 \
                     (lower),                                                  \
                     (upper),                                                  \
                     #actual,                                                  \
                     #lower,                                                   \
                     #upper,                                                   \
                     "CHECK_IN_BOUNDS",                                        \
                     __FILE__,                                                 \
                     __LINE__)
#define CHECK_IN_RANGE(actual, lower, upper)                                   \
  Test::CheckInRange((actual),                                                 \
                     (lower),                                                  \
                     (upper),                                                  \
                     #actual,                                                  \
                     #lower,                                                   \
                     #upper,                                                   \
                     "CHECK_IN_RANGE",                                         \
                     __FILE__,                                                 \
                     __LINE__)

#define TEST_MAIN()                                                            \
  int main()                                                                   \
  {                                                                            \
    return Test::RunAll();                                                     \
  }
