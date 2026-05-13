
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
using TSetup = music::Setup<>;
using TChordEventSet = music::ChordEventSet<>;
using FRange = Range<float>;

template<std::size_t SCALE_DEGREES = music::DEF_SCALE_DEGREES>
struct Role1
{
  const MString<6> label = "Role1";
  const int octave_offset;
  const FRange density;
  const FRange repeat_probability{ 0.0F, 0.0F };
  const NoteValue granularity = NoteValue::Quarter;
  const music::WeightMap<SCALE_DEGREES>& weight_map =
    music::SCALE_WEIGHTS_7_UNIFORM;

  template<std::size_t MAX_EVENTS>
  using PatternGenerator = EuclidianPatternGenerator<MAX_EVENTS>;

  Role1(int o, float dl, float dh)
    : octave_offset(o)
    , density(dl, dh) {};
};

template<std::size_t SCALE_DEGREES = music::DEF_SCALE_DEGREES>
struct Role2
{
  const MString<6> label = "Role2";
  const int octave_offset;
  const FRange density;
  const FRange repeat_probability{ 0.0F, 0.0F };
  const NoteValue granularity = NoteValue::Quarter;
  const music::WeightMap<SCALE_DEGREES>& weight_map =
    music::SCALE_WEIGHTS_7_UNIFORM;

  Role2(int o, float dl, float dh)
    : octave_offset(o)
    , density(dl, dh) {};
};

TSetup setup(4, NoteValue::Quarter, 12.0F, 2.0F);

const float dLow = 0.0F;
const float dHigh = 0.9F;
Role1<> role(-1, dLow, dHigh);
Role2<> roleWithoutPatternGenerator(0, dLow, dHigh);

TEST_CASE("Persona Name Test")
{
  Persona<Role1<>> p("Persona", setup, role);
  CHECK_EQ(p.get_persona_name(), "Persona");
}

TEST_CASE("Role Name Test")
{
  Persona<Role1<>> p("P", setup, role);
  CHECK_EQ(p.GetRoleName(), "Role1");
}

TEST_CASE("Density Test")
{
  Persona<Role1<>> p("P", setup, role);
  CHECK_IN_RANGE(p.get_density(), dLow, dHigh);
}

TEST_CASE("Generator Injection Syntax Test")
{
  Persona<Role1<>> p("P", setup, role);
  TChordEventSet chords;
  music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents<EuclidianPatternGenerator>(chords, events),
           0UL);
}

TEST_CASE("Role Default Generator Syntax Test")
{
  Persona<Role1<>> p("P", setup, role);
  TChordEventSet chords;
  music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents(chords, events), 0UL);
}

TEST_CASE("Default Fallback Generator Syntax Test")
{
  Persona<Role2<>> p("P", setup, roleWithoutPatternGenerator);
  TChordEventSet chords;
  music::NoteEventSet<> events;
  CHECK_EQ(p.GenerateNoteEvents(chords, events), 0UL);
}

static_assert(std::is_same_v<Persona<Role1<>>::TDefaultPatternGenerator,
                             EuclidianPatternGenerator<music::DEF_MAX_EVENTS>>);
static_assert(
  std::is_same_v<Persona<Role2<>>::TDefaultPatternGenerator,
                 RandomRandomPatternGenerator<music::DEF_MAX_EVENTS>>);

TEST_MAIN();
