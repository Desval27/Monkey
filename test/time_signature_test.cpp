#include <music/time_signature.hpp>

#include "test_framework.hpp"

using music::NoteValue;
using music::TimeSignature;
using music::TimeSignatureType;

static int
TypeValue(TimeSignatureType type)
{
  return static_cast<int>(type);
}

TEST_CASE("TimeSignature defaults to common time")
{
  TimeSignature ts;

  CHECK_EQ(ts.beats, 4);
  CHECK_EQ(ts.beatValue, NoteValue::Quarter);
  CHECK_EQ(TypeValue(ts.get_type()), TypeValue(TimeSignatureType::Common));
}

TEST_CASE("TimeSignature stores constructor values")
{
  TimeSignature ts(7, NoteValue::Eighth);

  CHECK_EQ(ts.beats, 7);
  CHECK_EQ(ts.beatValue, NoteValue::Eighth);
}

TEST_CASE("TimeSignature copy preserves meter")
{
  TimeSignature original(6, NoteValue::Eighth);
  TimeSignature copy(original);

  CHECK_EQ(copy.beats, 6);
  CHECK_EQ(copy.beatValue, NoteValue::Eighth);
  CHECK_EQ(TypeValue(copy.get_type()), TypeValue(TimeSignatureType::Compound));
}

TEST_CASE(
  "TimeSignature classifies common simple compound and irregular meters")
{
  CHECK_EQ(TypeValue(TimeSignature(4, NoteValue::Quarter).get_type()),
           TypeValue(TimeSignatureType::Common));
  CHECK_EQ(TypeValue(TimeSignature(3, NoteValue::Quarter).get_type()),
           TypeValue(TimeSignatureType::Simple));
  CHECK_EQ(TypeValue(TimeSignature(2, NoteValue::Quarter).get_type()),
           TypeValue(TimeSignatureType::Simple));
  CHECK_EQ(TypeValue(TimeSignature(6, NoteValue::Eighth).get_type()),
           TypeValue(TimeSignatureType::Compound));
  CHECK_EQ(TypeValue(TimeSignature(9, NoteValue::Eighth).get_type()),
           TypeValue(TimeSignatureType::Compound));
  CHECK_EQ(TypeValue(TimeSignature(12, NoteValue::Eighth).get_type()),
           TypeValue(TimeSignatureType::Compound));
  CHECK_EQ(TypeValue(TimeSignature(5, NoteValue::Quarter).get_type()),
           TypeValue(TimeSignatureType::Irregular));
  CHECK_EQ(TypeValue(TimeSignature(7, NoteValue::Eighth).get_type()),
           TypeValue(TimeSignatureType::Irregular));
}

TEST_CASE("TimeSignature denominator is derived from beat value")
{
  CHECK_EQ(TimeSignature(4, NoteValue::Whole).get_denominator(), 1);
  CHECK_EQ(TimeSignature(4, NoteValue::Half).get_denominator(), 2);
  CHECK_EQ(TimeSignature(4, NoteValue::Quarter).get_denominator(), 4);
  CHECK_EQ(TimeSignature(6, NoteValue::Eighth).get_denominator(), 8);
  CHECK_EQ(TimeSignature(4, NoteValue::Sixteenth).get_denominator(), 16);
}

TEST_CASE("TimeSignature denominator is zero for missing beat value")
{
  TimeSignature ts(4, NoteValue::None);

  CHECK_EQ(ts.get_denominator(), 0);
}

TEST_CASE("TimeSignature pulses per bar multiplies beats by beat value")
{
  CHECK_EQ(TimeSignature(4, NoteValue::Quarter).get_pulses_per_bar(), 4 * PPQN);
  CHECK_EQ(TimeSignature(6, NoteValue::Eighth).get_pulses_per_bar(), 3 * PPQN);
  CHECK_EQ(TimeSignature(7, NoteValue::Eighth).get_pulses_per_bar(),
           7 * static_cast<int>(NoteValue::Eighth));
}

TEST_CASE("TimeSignature pulses per bar reflects invalid inputs without hiding "
          "them")
{
  CHECK_EQ(TimeSignature(0, NoteValue::Quarter).get_pulses_per_bar(), 0);
  CHECK_EQ(TimeSignature(4, NoteValue::None).get_pulses_per_bar(), 0);
}

TEST_CASE("TimeSignature is in threes")
{
  CHECK_EQ(TimeSignature(2, NoteValue::Half).in_threes(), false);
  CHECK_EQ(TimeSignature(3, NoteValue::Half).in_threes(), true);
  CHECK_EQ(TimeSignature(3, NoteValue::Quarter).in_threes(), true);
  CHECK_EQ(TimeSignature(4, NoteValue::Quarter).in_threes(), false);
  CHECK_EQ(TimeSignature(5, NoteValue::Quarter).in_threes(), false);
  CHECK_EQ(TimeSignature(3, NoteValue::Eighth).in_threes(), true);
  CHECK_EQ(TimeSignature(5, NoteValue::Eighth).in_threes(), false);
  CHECK_EQ(TimeSignature(6, NoteValue::Eighth).in_threes(), true);
  CHECK_EQ(TimeSignature(7, NoteValue::Eighth).in_threes(), false);
  CHECK_EQ(TimeSignature(9, NoteValue::Eighth).in_threes(), true);
  CHECK_EQ(TimeSignature(12, NoteValue::Eighth).in_threes(), true);
  CHECK_EQ(TimeSignature(12, NoteValue::Sixteenth).in_threes(), true);
}

TEST_MAIN()
