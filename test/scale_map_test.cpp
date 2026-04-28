#include "TestFramework.h"

#include <Music/Music.h>

using Music::Degree;
using Music::DEF_MAX_DEGREES;
using Music::ScaleMap;

TEST_CASE("ScaleMap default map behavior") {
  ScaleMap map;
  int period = 42;
  const Music::WeightMap<> weights = {1.0f, 1.0f, 1.0f, 1.0f,
                                      1.0f, 1.0f, 1.0f};

  CHECK_EQ(map.Count(), std::size_t(Music::DEF_SCALE_DEGREES));

  CHECK_EQ(map.GetMappedDegree(3, period), Degree(0));
  CHECK_EQ(period, 0);

  period = 42;
  CHECK_EQ(map.GetWeightedDegree(0.5f, period, weights), Degree(0));
  CHECK_EQ(period, 0);

  period = 42;
  CHECK_EQ(map.GetWeightedMappedDegree(3, 0.5f, period, weights),
           Degree(0));
  CHECK_EQ(period, 0);
}

TEST_CASE("ScaleMap SetDegrees validates input and wraps indices") {
  ScaleMap<Music::PENTATONIC> map;
  const Music::DegreeMap<Music::PENTATONIC> degrees = {0, 2, 4, 5, 7};
  int period = 99;

  map.SetDegrees(degrees);
  CHECK_EQ(map.Count(), std::size_t(5));

  CHECK_EQ(map.GetMappedDegree(0, period), Degree(0));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetMappedDegree(4, period), Degree(7));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetMappedDegree(5, period), Degree(0));
  CHECK_EQ(period, 1);

  CHECK_EQ(map.GetMappedDegree(9, period), Degree(7));
  CHECK_EQ(period, 1);
}

TEST_CASE("ScaleMap GetIndexOfDegree returns first match") {
  ScaleMap<4> map;
  const Music::DegreeMap<4> degrees = {0, 2, 2, 5};
  map.SetDegrees(degrees);

  CHECK_EQ(map.GetIndexOfDegree(Degree(0)), std::size_t(0));
  CHECK_EQ(map.GetIndexOfDegree(Degree(2)), std::size_t(1));
  CHECK_EQ(map.GetIndexOfDegree(Degree(9)), std::size_t(0));
}

TEST_CASE("ScaleMap weighted selection respects weights and aliases") {
  ScaleMap<Music::PENTATONIC> map;
  const Music::DegreeMap<Music::PENTATONIC> degrees = {0, 2, 4, 5, 7};
  const Music::WeightMap<Music::PENTATONIC> singlePickWeights = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
  int period = 99;

  map.SetDegrees(degrees);

  CHECK_EQ(map.GetWeightedDegree(0.0f, period, singlePickWeights),
           Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedDegree(0.5f, period, singlePickWeights),
           Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedMappedDegree(8, 0.75f, period, singlePickWeights),
           Degree(4));
  CHECK_EQ(period, 1);

  CHECK_EQ(map.GetWeightedNote(0.25f, period, singlePickWeights),
           Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedNote(4, 0.25f, period, singlePickWeights),
           Degree(4));
  CHECK_EQ(period, 0);
}

TEST_CASE("ScaleMap zero-sum weights fall back to uniform selection") {
  ScaleMap<Music::PENTATONIC> map;
  const Music::DegreeMap<Music::PENTATONIC> degrees = {0, 2, 4, 5, 7};
  const Music::WeightMap<Music::PENTATONIC> zeroWeights = {
      0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  int period = 99;

  map.SetDegrees(degrees);

  CHECK_EQ(map.GetWeightedDegree(-1.0f, period, zeroWeights), Degree(0));
  CHECK_EQ(map.GetWeightedDegree(0.40f, period, zeroWeights), Degree(4));
  CHECK_EQ(map.GetWeightedDegree(1.0f, period, zeroWeights), Degree(7));
}

TEST_CASE("ScaleMap truncates oversized degree arrays") {
  ScaleMap<DEF_MAX_DEGREES> map;
  Music::DegreeMap<DEF_MAX_DEGREES> degrees = {};
  int period = 99;

  for (std::size_t i = 0; i < DEF_MAX_DEGREES; ++i) {
    degrees[i] = static_cast<Degree>(i);
  }

  map.SetDegrees(degrees);
  CHECK_EQ(map.Count(), DEF_MAX_DEGREES);

  CHECK_EQ(map.GetMappedDegree(DEF_MAX_DEGREES - 1, period),
           static_cast<Degree>(DEF_MAX_DEGREES - 1));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetMappedDegree(DEF_MAX_DEGREES, period), Degree(0));
  CHECK_EQ(period, 1);
}

TEST_MAIN();
