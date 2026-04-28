#include "TestFramework.h"

#include <Music/TimeSignature.h>

using Music::NoteValue;
using Music::TimeSignature;
using Music::TimeSignatureType;

static int TypeValue(TimeSignatureType type)
{
  return static_cast<int>(type);
}

TEST_CASE("TimeSignature defaults to common time")
{
  TimeSignature ts;

  CHECK_EQ(ts.beats, 4);
  CHECK_EQ(ts.beatValue, NoteValue::Quarter);
  CHECK_EQ(TypeValue(ts.GetType()), TypeValue(TimeSignatureType::Common));
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
  CHECK_EQ(TypeValue(copy.GetType()), TypeValue(TimeSignatureType::Compound));
}

TEST_CASE("TimeSignature classifies common simple compound and irregular meters")
{
  CHECK_EQ(TypeValue(TimeSignature(4, NoteValue::Quarter).GetType()),
           TypeValue(TimeSignatureType::Common));
  CHECK_EQ(TypeValue(TimeSignature(3, NoteValue::Quarter).GetType()),
           TypeValue(TimeSignatureType::Simple));
  CHECK_EQ(TypeValue(TimeSignature(2, NoteValue::Quarter).GetType()),
           TypeValue(TimeSignatureType::Simple));
  CHECK_EQ(TypeValue(TimeSignature(6, NoteValue::Eighth).GetType()),
           TypeValue(TimeSignatureType::Compound));
  CHECK_EQ(TypeValue(TimeSignature(9, NoteValue::Eighth).GetType()),
           TypeValue(TimeSignatureType::Compound));
  CHECK_EQ(TypeValue(TimeSignature(12, NoteValue::Eighth).GetType()),
           TypeValue(TimeSignatureType::Compound));
  CHECK_EQ(TypeValue(TimeSignature(5, NoteValue::Quarter).GetType()),
           TypeValue(TimeSignatureType::Irregular));
  CHECK_EQ(TypeValue(TimeSignature(7, NoteValue::Eighth).GetType()),
           TypeValue(TimeSignatureType::Irregular));
}

TEST_CASE("TimeSignature denominator is derived from beat value")
{
  CHECK_EQ(TimeSignature(4, NoteValue::Whole).GetDenominator(), 1);
  CHECK_EQ(TimeSignature(4, NoteValue::Half).GetDenominator(), 2);
  CHECK_EQ(TimeSignature(4, NoteValue::Quarter).GetDenominator(), 4);
  CHECK_EQ(TimeSignature(6, NoteValue::Eighth).GetDenominator(), 8);
  CHECK_EQ(TimeSignature(4, NoteValue::Sixteenth).GetDenominator(), 16);
}

TEST_CASE("TimeSignature denominator is zero for missing beat value")
{
  TimeSignature ts(4, NoteValue::None);

  CHECK_EQ(ts.GetDenominator(), 0);
}

TEST_CASE("TimeSignature pulses per bar multiplies beats by beat value")
{
  CHECK_EQ(TimeSignature(4, NoteValue::Quarter).GetPulsesPerBar(), 4 * PPQN);
  CHECK_EQ(TimeSignature(6, NoteValue::Eighth).GetPulsesPerBar(), 3 * PPQN);
  CHECK_EQ(TimeSignature(7, NoteValue::Eighth).GetPulsesPerBar(),
           7 * static_cast<int>(NoteValue::Eighth));
}

TEST_CASE("TimeSignature pulses per bar reflects invalid inputs without hiding them")
{
  CHECK_EQ(TimeSignature(0, NoteValue::Quarter).GetPulsesPerBar(), 0);
  CHECK_EQ(TimeSignature(4, NoteValue::None).GetPulsesPerBar(), 0);
}

TEST_MAIN()
