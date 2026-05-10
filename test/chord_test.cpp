#include <music/music.hpp>

#include "test_framework.hpp"

using music::ChordAlteration;
using music::ChordEvent;
using music::ChordExtension;
using music::Degree;
using music::NoteValue;
using music::ScaleMap;
using music::Temperament;

static void
CheckChordName(const ChordEvent<>& chord,
               const ScaleMap<>& scale,
               const char* expected)
{
  MString<16> name;
  CHECK(chord.get_chord_name(scale, name));
  CHECK_EQ(name.c_str(), expected);
}

template<std::size_t TEMPERAMENT_DEGREES>
static void
CheckChordName(const ChordEvent<>& chord,
               const ScaleMap<>& scale,
               const Temperament<TEMPERAMENT_DEGREES>& temperament,
               const char* expected)
{
  MString<16> name;
  CHECK(chord.get_chord_name(scale, temperament, name));
  CHECK_EQ(name.c_str(), expected);
}

TEST_CASE("Chord names major-scale triads with roman numeral quality")
{
  ScaleMap scale;
  const music::DegreeMap<> ionian = { 0, 2, 4, 5, 7, 9, 11 };
  scale.set_degrees(ionian);

  CheckChordName(ChordEvent(0, NoteValue::Whole), scale, "I");
  CheckChordName(ChordEvent(2, NoteValue::Whole), scale, "ii");
  CheckChordName(ChordEvent(4, NoteValue::Whole), scale, "iii");
  CheckChordName(ChordEvent(5, NoteValue::Whole), scale, "IV");
  CheckChordName(ChordEvent(7, NoteValue::Whole), scale, "V");
  CheckChordName(ChordEvent(9, NoteValue::Whole), scale, "vi");
  CheckChordName(ChordEvent(11, NoteValue::Whole), scale, "vii_o");
}

TEST_CASE("Chord names minor-scale triads from the present chord tones")
{
  ScaleMap scale;
  const music::DegreeMap<> aeolian = { 0, 2, 3, 5, 7, 8, 10 };
  scale.set_degrees(aeolian);

  CheckChordName(ChordEvent(0, NoteValue::Whole), scale, "i");
  CheckChordName(ChordEvent(2, NoteValue::Whole), scale, "ii_o");
  CheckChordName(ChordEvent(3, NoteValue::Whole), scale, "III");
  CheckChordName(ChordEvent(5, NoteValue::Whole), scale, "iv");
  CheckChordName(ChordEvent(7, NoteValue::Whole), scale, "v");
  CheckChordName(ChordEvent(8, NoteValue::Whole), scale, "VI");
  CheckChordName(ChordEvent(10, NoteValue::Whole), scale, "VII");
}

TEST_CASE(
  "Chord names reflect raised-scale tones when the triad changes quality")
{
  ScaleMap scale;
  const music::DegreeMap<> harmonicMinor = { 0, 2, 3, 5, 7, 8, 11 };
  scale.set_degrees(harmonicMinor);

  CheckChordName(ChordEvent(7, NoteValue::Whole), scale, "V");
  CheckChordName(ChordEvent(11, NoteValue::Whole), scale, "vii_o");
}

TEST_CASE("Chord name returns unknown when the root is outside the scale")
{
  ScaleMap scale;
  const music::DegreeMap<> ionian = { 0, 2, 4, 5, 7, 9, 11 };
  scale.set_degrees(ionian);

  CheckChordName(ChordEvent(1, NoteValue::Whole), scale, "?");
}

TEST_CASE("Chord tones carry into the next period for wrapped triads")
{
  ScaleMap scale;
  const music::DegreeMap<> ionian = { 0, 2, 4, 5, 7, 9, 11 };
  music::Note tones[3] = {};
  scale.set_degrees(ionian);

  const std::size_t toneCount =
    ChordEvent(11, NoteValue::Whole).get_chord_tones(scale, 12, tones, 3);

  CHECK_EQ(toneCount, std::size_t(3));
  CHECK_EQ(tones[0], music::Note(11));
  CHECK_EQ(tones[1], music::Note(14));
  CHECK_EQ(tones[2], music::Note(17));
}

TEST_CASE("Chord extensions add scale-stacked chord tones")
{
  ScaleMap scale;
  const music::DegreeMap<> ionian = { 0, 2, 4, 5, 7, 9, 11 };
  music::Note tones[6] = {};
  scale.set_degrees(ionian);

  const ChordEvent chord(
    0, NoteValue::Whole, ChordExtension::Seventh | ChordExtension::Ninth);
  const std::size_t toneCount = chord.get_chord_tones(scale, 12, tones, 6);

  CHECK_EQ(toneCount, std::size_t(5));
  CHECK_EQ(tones[0], music::Note(0));
  CHECK_EQ(tones[1], music::Note(4));
  CHECK_EQ(tones[2], music::Note(7));
  CHECK_EQ(tones[3], music::Note(11));
  CHECK_EQ(tones[4], music::Note(14));
  CheckChordName(chord, scale, "I9");
}

TEST_CASE("Chord alterations adjust generated chord tones and names")
{
  ScaleMap scale;
  const music::DegreeMap<> ionian = { 0, 2, 4, 5, 7, 9, 11 };
  music::Note tones[5] = {};
  scale.set_degrees(ionian);

  const ChordEvent chord(7,
                         NoteValue::Whole,
                         ChordExtension::Seventh | ChordExtension::Ninth,
                         ChordAlteration::Sharp5 | ChordAlteration::Flat9);
  const std::size_t toneCount = chord.get_chord_tones(scale, 12, tones, 5);

  CHECK_EQ(toneCount, std::size_t(5));
  CHECK_EQ(tones[0], music::Note(7));
  CHECK_EQ(tones[1], music::Note(11));
  CHECK_EQ(tones[2], music::Note(15));
  CHECK_EQ(tones[3], music::Note(17));
  CHECK_EQ(tones[4], music::Note(20));
  CheckChordName(chord, scale, "V9#5b9");
}

TEST_CASE("Chord sus extensions replace the third")
{
  ScaleMap scale;
  const music::DegreeMap<> ionian = { 0, 2, 4, 5, 7, 9, 11 };
  music::Note tones[3] = {};
  scale.set_degrees(ionian);

  const ChordEvent chord(0, NoteValue::Whole, ChordExtension::Sus4);
  const std::size_t toneCount = chord.get_chord_tones(scale, 12, tones, 3);

  CHECK_EQ(toneCount, std::size_t(3));
  CHECK_EQ(tones[0], music::Note(0));
  CHECK_EQ(tones[1], music::Note(5));
  CHECK_EQ(tones[2], music::Note(7));
  CheckChordName(chord, scale, "Isus4");
}

TEST_CASE("Chord inversion rotates chord tones by period")
{
  ScaleMap scale;
  const music::DegreeMap<> ionian = { 0, 2, 4, 5, 7, 9, 11 };
  music::Note tones[4] = {};
  scale.set_degrees(ionian);

  const ChordEvent chord(
    0, NoteValue::Whole, ChordExtension::Seventh, ChordAlteration::None, 1);
  const std::size_t toneCount = chord.get_chord_tones(scale, 12, tones, 4);

  CHECK_EQ(toneCount, std::size_t(4));
  CHECK_EQ(tones[0], music::Note(4));
  CHECK_EQ(tones[1], music::Note(7));
  CHECK_EQ(tones[2], music::Note(11));
  CHECK_EQ(tones[3], music::Note(12));
}

TEST_CASE("Chord naming uses the temperament period when provided")
{
  ScaleMap scale;
  Temperament<> temperament;
  music::Note tones[3] = {};
  CHECK(temperament.make_equal_division(15, 2.0F));
  CHECK(temperament.attach_interval_labels(music::INTERVAL_NAMES_15));
  scale.set_degrees(music::IONIAN_D15);

  CheckChordName(ChordEvent(0, NoteValue::Whole), scale, temperament, "I");
  CheckChordName(ChordEvent(2, NoteValue::Whole), scale, temperament, "ii");

  const ChordEvent chord(14, NoteValue::Whole);
  const std::size_t toneCount = chord.get_chord_tones(
    scale, static_cast<int>(temperament.degrees_per_period()), tones, 3);

  CHECK_EQ(toneCount, std::size_t(3));
  CHECK_EQ(tones[0], music::Note(14));
  CHECK_EQ(tones[1], music::Note(17));
  CHECK_EQ(tones[2], music::Note(21));
  CheckChordName(chord, scale, temperament, "vii");
}

TEST_MAIN();
