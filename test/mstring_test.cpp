#include <sstream>
#include <string>

#include <monkey.hpp>
#include <mstring.hpp>

#include "test_framework.hpp"

TEST_CASE("MString stores and exposes a null-terminated string")
{
  MString<8> text("hello");

  CHECK_EQ(text.c_str(), "hello");
  CHECK_EQ(text.size(), 5UL);
  CHECK_EQ(text.capacity(), 8UL);
  CHECK(!text.empty());
}

TEST_CASE("MString truncates safely")
{
  MString<3> text("abcdef");

  CHECK_EQ(text.c_str(), "abc");
  CHECK_EQ(text.size(), 3UL);
  CHECK(!text.set("wxyz"));
  CHECK_EQ(text.c_str(), "wxy");
}

TEST_CASE("MString clears on null input")
{
  MString<4> text("note");

  CHECK(text.set(nullptr));
  CHECK_EQ(text.c_str(), "");
  CHECK(text.empty());
}

TEST_CASE("MString formats into fixed storage")
{
  MString<8> text;
  const int written = text.printf("%s-%d", "bar", 12);

  CHECK_EQ(written, 6);
  CHECK_EQ(text.c_str(), "bar-12");
}

TEST_CASE("MString appends into remaining storage")
{
  MString<8> text("bar");

  CHECK(text.append("-12"));
  CHECK_EQ(text.c_str(), "bar-12");
  CHECK_EQ(text.size(), 6UL);
}

TEST_CASE("MString append truncates safely")
{
  MString<5> text("abc");

  CHECK(!text.append("def"));
  CHECK_EQ(text.c_str(), "abcde");
  CHECK_EQ(text.size(), 5UL);
}

TEST_CASE("MString append ignores null input")
{
  MString<5> text("abc");

  CHECK(text.append(nullptr));
  CHECK_EQ(text.c_str(), "abc");
}

TEST_CASE("MString compares by string contents")
{
  MString<8> a("abc");
  MString<8> b("abd");
  MString<8> c("abc");

  CHECK(a == c);
  CHECK(a != b);
  CHECK(a < b);
  CHECK(b > a);
}

TEST_CASE("MString streams as string contents")
{
  MString<8> text("abc");
  std::ostringstream out;

  out << text;
  CHECK_EQ(out.str(), std::string("abc"));
}

TEST_MAIN()
