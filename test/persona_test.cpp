
#include <Music/Music.h>

#include "TestFramework.h"

using Music::NoteValue;
using Music::Persona;
using Music::TimeSignature;
using Music::Temperament;
using Music::ScaleMap;
using Music::EuclidianGenerator;

using FRange = Range<float>;

struct Role1 {
  static constexpr const char Name[] = "Role1";
  const FRange density;
  const NoteValue granularity = NoteValue::Quarter;
  Role1(float dl, float dh) : density(dl, dh) {};
};

TimeSignature ts(4, NoteValue::Quarter);
Temperament t;
ScaleMap s;

const float dLow = 0.0f;
const float dHigh = 0.9f;
Role1 role(dLow, dHigh);

TEST_CASE("Name Test") {
  Persona<Role1> p(ts, t, s, role);  
  CHECK_EQ(p.GetName(), "Role1");
}

TEST_CASE("Density Test") {
  Persona<Role1> p(ts, t, s, role);  
  CHECK_IN_RANGE(p.GetDensity(), dLow, dHigh);
}

TEST_CASE("Generator Injection Syntax Test") {
  Persona<Role1> p(ts, t, s, role);
  Music::ChordEventSet<> chords;
  Music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents<EuclidianGenerator>(chords, events), 0UL);
}

TEST_MAIN();
