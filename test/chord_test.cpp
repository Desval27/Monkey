#include "TestFramework.h"

#include <string>

#include <Music/Music.h>

using Music::ChordEvent;
using Music::Degree;
using Music::NoteValue;
using Music::ScaleMap;
using Music::Temperament;

TEST_CASE("Chord names major-scale triads with roman numeral quality") {
  ScaleMap scale;
  const Degree ionian[] = {0, 2, 4, 5, 7, 9, 11};
  CHECK(scale.SetDegrees(ionian, 7));

  CHECK_EQ(ChordEvent(0, NoteValue::Whole).GetChordName(scale), std::string("I"));
  CHECK_EQ(ChordEvent(2, NoteValue::Whole).GetChordName(scale), std::string("ii"));
  CHECK_EQ(ChordEvent(4, NoteValue::Whole).GetChordName(scale), std::string("iii"));
  CHECK_EQ(ChordEvent(5, NoteValue::Whole).GetChordName(scale), std::string("IV"));
  CHECK_EQ(ChordEvent(7, NoteValue::Whole).GetChordName(scale), std::string("V"));
  CHECK_EQ(ChordEvent(9, NoteValue::Whole).GetChordName(scale), std::string("vi"));
  CHECK_EQ(ChordEvent(11, NoteValue::Whole).GetChordName(scale), std::string("vii_o"));
}

TEST_CASE("Chord names minor-scale triads from the present chord tones") {
  ScaleMap scale;
  const Degree aeolian[] = {0, 2, 3, 5, 7, 8, 10};
  CHECK(scale.SetDegrees(aeolian, 7));

  CHECK_EQ(ChordEvent(0, NoteValue::Whole).GetChordName(scale), std::string("i"));
  CHECK_EQ(ChordEvent(2, NoteValue::Whole).GetChordName(scale), std::string("ii_o"));
  CHECK_EQ(ChordEvent(3, NoteValue::Whole).GetChordName(scale), std::string("III"));
  CHECK_EQ(ChordEvent(5, NoteValue::Whole).GetChordName(scale), std::string("iv"));
  CHECK_EQ(ChordEvent(7, NoteValue::Whole).GetChordName(scale), std::string("v"));
  CHECK_EQ(ChordEvent(8, NoteValue::Whole).GetChordName(scale), std::string("VI"));
  CHECK_EQ(ChordEvent(10, NoteValue::Whole).GetChordName(scale), std::string("VII"));
}

TEST_CASE("Chord names reflect raised-scale tones when the triad changes quality") {
  ScaleMap scale;
  const Degree harmonicMinor[] = {0, 2, 3, 5, 7, 8, 11};
  CHECK(scale.SetDegrees(harmonicMinor, 7));

  CHECK_EQ(ChordEvent(7, NoteValue::Whole).GetChordName(scale), std::string("V"));
  CHECK_EQ(ChordEvent(11, NoteValue::Whole).GetChordName(scale), std::string("vii_o"));
}

TEST_CASE("Chord name returns unknown when the root is outside the scale") {
  ScaleMap scale;
  const Degree ionian[] = {0, 2, 4, 5, 7, 9, 11};
  CHECK(scale.SetDegrees(ionian, 7));

  CHECK_EQ(ChordEvent(1, NoteValue::Whole).GetChordName(scale), std::string("?"));
}

TEST_CASE("Chord tones carry into the next period for wrapped triads") {
  ScaleMap scale;
  const Degree ionian[] = {0, 2, 4, 5, 7, 9, 11};
  Music::Note tones[3] = {};
  CHECK(scale.SetDegrees(ionian, 7));

  const size_t toneCount =
      ChordEvent(11, NoteValue::Whole).GetChordTones(scale, 12, tones, 3);

  CHECK_EQ(toneCount, size_t(3));
  CHECK_EQ(tones[0], Music::Note(11));
  CHECK_EQ(tones[1], Music::Note(14));
  CHECK_EQ(tones[2], Music::Note(17));
}

TEST_CASE("Chord naming uses the temperament period when provided") {
  ScaleMap scale;
  Temperament temperament;
  Music::Note tones[3] = {};
  CHECK(temperament.MakeEqualDivision(15, 2.0f));
  CHECK(temperament.AttachIntervalLabels(Music::INTERVAL_NAMES_15,
                                         ArrayLen(Music::INTERVAL_NAMES_15)));
  CHECK(scale.SetDegrees(Music::IONIAN_D15, ArrayLen(Music::IONIAN_D15)));

  CHECK_EQ(ChordEvent(0, NoteValue::Whole).GetChordName(scale, temperament),
           std::string("I"));
  CHECK_EQ(ChordEvent(2, NoteValue::Whole).GetChordName(scale, temperament),
           std::string("ii"));

  const ChordEvent chord(14, NoteValue::Whole);
  const size_t toneCount = chord.GetChordTones(
      scale, static_cast<int>(temperament.DegreesPerPeriod()), tones, 3);

  CHECK_EQ(toneCount, size_t(3));
  CHECK_EQ(tones[0], Music::Note(14));
  CHECK_EQ(tones[1], Music::Note(17));
  CHECK_EQ(tones[2], Music::Note(21));
  CHECK_EQ(chord.GetChordName(scale, temperament), std::string("vii"));
}

TEST_MAIN();
