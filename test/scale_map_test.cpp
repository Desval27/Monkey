#include <Music/Music.h>

#include "TestFramework.h"

using Music::DEF_MAX_DEGREES;
using Music::Degree;
using Music::HarmonicMode;
using Music::ScaleMap;

TEST_CASE("ScaleMap default map behavior")
{
  ScaleMap map;
  int period = 42;
  const Music::WeightMap<> weights = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
  };

  CHECK_EQ(map.size(), std::size_t(Music::DEF_SCALE_DEGREES));
  CHECK(map.GetHarmonicMode() == HarmonicMode::Major);

  CHECK_EQ(map.get_mapped_degree(3, period), Degree(0));
  CHECK_EQ(period, 0);

  period = 42;
  CHECK_EQ(map.GetWeightedDegree(0.5f, period, weights), Degree(0));
  CHECK_EQ(period, 0);

  period = 42;
  CHECK_EQ(map.GetWeightedMappedDegree(3, 0.5f, period, weights), Degree(0));
  CHECK_EQ(period, 0);
}

TEST_CASE("ScaleMap set_degrees validates input and wraps indices")
{
  ScaleMap<Music::PENTATONIC> map;
  const Music::DegreeMap<Music::PENTATONIC> degrees = { 0, 2, 4, 5, 7 };
  int period = 99;

  map.set_degrees(degrees);
  CHECK_EQ(map.size(), std::size_t(5));

  CHECK_EQ(map.get_mapped_degree(0, period), Degree(0));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.get_mapped_degree(4, period), Degree(7));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.get_mapped_degree(5, period), Degree(0));
  CHECK_EQ(period, 1);

  CHECK_EQ(map.get_mapped_degree(9, period), Degree(7));
  CHECK_EQ(period, 1);
}

TEST_CASE("ScaleMap tracks harmonic mode from degree maps")
{
  ScaleMap map;

  map.set_degrees(Music::IONIAN_D12);
  CHECK(map.GetHarmonicMode() == HarmonicMode::Major);

  map.set_degrees(Music::AEOLIAN_D12);
  CHECK(map.GetHarmonicMode() == HarmonicMode::Minor);

  map.set_degrees(Music::PHRYGIAN_DOMINANT_D12, HarmonicMode::Minor);
  CHECK(map.GetHarmonicMode() == HarmonicMode::Minor);
}

TEST_CASE("ScaleMap can load harmonic mode from scale table metadata")
{
  ScaleMap map;
  int period = 99;

  map.set_scale(Music::HEPATONIC_D12_SCALES[5]);

  CHECK_EQ(map.get_mapped_degree(2, period), Degree(3));
  CHECK_EQ(period, 0);
  CHECK(map.GetHarmonicMode() == HarmonicMode::Minor);
}

TEST_CASE("ScaleMap GetIndexOfDegree returns first match")
{
  ScaleMap<4> map;
  const Music::DegreeMap<4> degrees = { 0, 2, 2, 5 };
  map.set_degrees(degrees);

  CHECK_EQ(map.get_index_of_degree(Degree(0)), std::size_t(0));
  CHECK_EQ(map.get_index_of_degree(Degree(2)), std::size_t(1));
  CHECK_EQ(map.get_index_of_degree(Degree(9)), std::size_t(0));
}

TEST_CASE("ScaleMap weighted selection respects weights and aliases")
{
  ScaleMap<Music::PENTATONIC> map;
  const Music::DegreeMap<Music::PENTATONIC> degrees = { 0, 2, 4, 5, 7 };
  const Music::WeightMap<Music::PENTATONIC> singlePickWeights = {
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f
  };
  int period = 99;

  map.set_degrees(degrees);

  CHECK_EQ(map.GetWeightedDegree(0.0f, period, singlePickWeights), Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedDegree(0.5f, period, singlePickWeights), Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedMappedDegree(8, 0.75f, period, singlePickWeights),
           Degree(4));
  CHECK_EQ(period, 1);

  CHECK_EQ(map.GetWeightedNote(0.25f, period, singlePickWeights), Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedNote(4, 0.25f, period, singlePickWeights), Degree(4));
  CHECK_EQ(period, 0);
}

TEST_CASE("ScaleMap zero-sum weights fall back to uniform selection")
{
  ScaleMap<Music::PENTATONIC> map;
  const Music::DegreeMap<Music::PENTATONIC> degrees = { 0, 2, 4, 5, 7 };
  const Music::WeightMap<Music::PENTATONIC> zeroWeights = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f
  };
  int period = 99;

  map.set_degrees(degrees);

  CHECK_EQ(map.GetWeightedDegree(-1.0f, period, zeroWeights), Degree(0));
  CHECK_EQ(map.GetWeightedDegree(0.40f, period, zeroWeights), Degree(4));
  CHECK_EQ(map.GetWeightedDegree(1.0f, period, zeroWeights), Degree(7));
}

TEST_CASE("ScaleMap truncates oversized degree arrays")
{
  ScaleMap<DEF_MAX_DEGREES> map;
  Music::DegreeMap<DEF_MAX_DEGREES> degrees = {};
  int period = 99;

  for (std::size_t i = 0; i < DEF_MAX_DEGREES; ++i) {
    degrees[i] = static_cast<Degree>(i);
  }

  map.set_degrees(degrees);
  CHECK_EQ(map.size(), DEF_MAX_DEGREES);

  CHECK_EQ(map.get_mapped_degree(DEF_MAX_DEGREES - 1, period),
           static_cast<Degree>(DEF_MAX_DEGREES - 1));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.get_mapped_degree(DEF_MAX_DEGREES, period), Degree(0));
  CHECK_EQ(period, 1);
}

TEST_MAIN();
