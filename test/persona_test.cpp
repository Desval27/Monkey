
#include <Music/Music.h>

#include "TestFramework.h"

using Music::NoteValue;
using Music::Persona;
using Music::TimeSignature;
using Music::Temperament;
using Music::ScaleMap;
using Music::EuclidianPatternGenerator;
using Music::NullPatternGenerator;
using MySetup = Music::Setup<>;
using MyChordEventSet = Music::ChordEventSet<>;
using FRange = Range<float>;

struct Role1 {
  static constexpr const char Name[] = "Role1";
  const int octaveOffset;
  const FRange density;
  const NoteValue granularity = NoteValue::Quarter;
  template <std::size_t MAX_EVENTS>
  using PatternGenerator = EuclidianPatternGenerator<MAX_EVENTS>;

  Role1(int o, float dl, float dh) : octaveOffset(o), density(dl, dh) {};
};

struct Role2 {
  static constexpr const char Name[] = "Role2";
  const int octaveOffset;
  const FRange density;
  const NoteValue granularity = NoteValue::Quarter;

  Role2(int o, float dl, float dh) : octaveOffset(o), density(dl, dh) {};
};

MySetup setup(4, NoteValue::Quarter, 12.0f, 2.0f);

const float dLow = 0.0f;
const float dHigh = 0.9f;
Role1 role(-1, dLow, dHigh);
Role2 roleWithoutPatternGenerator(0, dLow, dHigh);

TEST_CASE("Name Test") {
  Persona<Role1> p(setup, role);  
  CHECK_EQ(p.GetName(), "Role1");
}

TEST_CASE("Density Test") {
  Persona<Role1> p(setup, role);  
  CHECK_IN_RANGE(p.GetDensity(), dLow, dHigh);
}

TEST_CASE("Generator Injection Syntax Test") {
  Persona<Role1> p(setup, role);
  MyChordEventSet chords;
  Music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents<EuclidianPatternGenerator>(chords, events), 0UL);
}

TEST_CASE("Role Default Generator Syntax Test") {
  Persona<Role1> p(setup, role);
  MyChordEventSet chords;
  Music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents(chords, events), 0UL);
}

TEST_CASE("Default Fallback Generator Syntax Test") {
  Persona<Role2> p(setup, roleWithoutPatternGenerator);
  MyChordEventSet chords;
  Music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents(chords, events), 0UL);
}

static_assert(std::is_same_v<typename Persona<Role1>::DefaultPatternGenerator,
                             EuclidianPatternGenerator<Music::DEF_MAX_EVENTS>>);
static_assert(std::is_same_v<typename Persona<Role2>::DefaultPatternGenerator,
                             NullPatternGenerator<Music::DEF_MAX_EVENTS>>);

TEST_MAIN();
