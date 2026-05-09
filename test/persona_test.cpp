
#include <music/music.hpp>
#include <music/pattern_generator.hpp>
#include <music/persona.hpp>

#include "test_framework.hpp"

using music::EuclidianPatternGenerator;
using music::NoteValue;
using music::Persona;
using music::RandomRandomPatternGenerator;
using music::ScaleMap;
using music::Temperament;
using music::TimeSignature;
using MySetup = music::Setup<>;
using MyChordEventSet = music::ChordEventSet<>;
using FRange = Range<float>;

struct Role1
{
  const MString<6> Name = "Role1";
  const int octaveOffset;
  const FRange density;
  const NoteValue granularity = NoteValue::Quarter;
  template<std::size_t MAX_EVENTS>
  using PatternGenerator = EuclidianPatternGenerator<MAX_EVENTS>;

  Role1(int o, float dl, float dh)
    : octaveOffset(o)
    , density(dl, dh) {};
};

struct Role2
{
  const MString<6> Name = "Role2";
  const int octaveOffset;
  const FRange density;
  const NoteValue granularity = NoteValue::Quarter;

  Role2(int o, float dl, float dh)
    : octaveOffset(o)
    , density(dl, dh) {};
};

MySetup setup(4, NoteValue::Quarter, 12.0f, 2.0f);

const float dLow = 0.0f;
const float dHigh = 0.9f;
Role1 role(-1, dLow, dHigh);
Role2 roleWithoutPatternGenerator(0, dLow, dHigh);

TEST_CASE("Persona Name Test")
{
  Persona<Role1> p("Persona", setup, role);
  CHECK_EQ(p.GetPersonaName(), "Persona");
}

TEST_CASE("Role Name Test")
{
  Persona<Role1> p("P", setup, role);
  CHECK_EQ(p.GetRoleName(), "Role1");
}

TEST_CASE("Density Test")
{
  Persona<Role1> p("P", setup, role);
  CHECK_IN_RANGE(p.get_density(), dLow, dHigh);
}

TEST_CASE("Generator Injection Syntax Test")
{
  Persona<Role1> p("P", setup, role);
  MyChordEventSet chords;
  music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents<EuclidianPatternGenerator>(chords, events),
           0UL);
}

TEST_CASE("Role Default Generator Syntax Test")
{
  Persona<Role1> p("P", setup, role);
  MyChordEventSet chords;
  music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents(chords, events), 0UL);
}

TEST_CASE("Default Fallback Generator Syntax Test")
{
  Persona<Role2> p("P", setup, roleWithoutPatternGenerator);
  MyChordEventSet chords;
  music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents(chords, events), 0UL);
}

static_assert(std::is_same_v<typename Persona<Role1>::DefaultPatternGenerator,
                             EuclidianPatternGenerator<music::DEF_MAX_EVENTS>>);
static_assert(
  std::is_same_v<typename Persona<Role2>::DefaultPatternGenerator,
                 RandomRandomPatternGenerator<music::DEF_MAX_EVENTS>>);

TEST_MAIN();
