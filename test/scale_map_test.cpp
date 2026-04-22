#include "TestFramework.h"

#include <Music/Music.h>

using Music::Degree;
using Music::MAX_DEGREES;
using Music::ScaleMap;

TEST_CASE("ScaleMap empty map behavior") {
  ScaleMap map;
  int period = 42;
  const float weights[] = {1.0f, 1.0f, 1.0f};

  CHECK_EQ(map.Count(), 0);

  CHECK_EQ(map.GetMappedDegree(3, period), Degree(0));
  CHECK_EQ(period, 0);

  period = 42;
  CHECK_EQ(map.GetWeightedDegree(0.5f, period, weights, 3), Degree(0));
  CHECK_EQ(period, 0);

  period = 42;
  CHECK_EQ(map.GetWeightedMappedDegree(-3, 0.5f, period, weights, 3),
           Degree(0));
  CHECK_EQ(period, 0);
}

TEST_CASE("ScaleMap SetDegrees validates input and wraps indices") {
  ScaleMap map;
  const Degree degrees[] = {0, 2, 4, 5, 7};
  int period = 99;

  CHECK(!map.SetDegrees(nullptr, 5));
  CHECK(!map.SetDegrees(degrees, 0));
  CHECK(map.SetDegrees(degrees, 5));
  CHECK_EQ(map.Count(), 5);

  CHECK_EQ(map.GetMappedDegree(0, period), Degree(0));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetMappedDegree(4, period), Degree(7));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetMappedDegree(5, period), Degree(0));
  CHECK_EQ(period, 1);

  CHECK_EQ(map.GetMappedDegree(-1, period), Degree(7));
  CHECK_EQ(period, -1);
}

TEST_CASE("ScaleMap GetIndexOfDegree returns first match") {
  ScaleMap map;
  const Degree degrees[] = {0, 2, 2, 5};
  CHECK(map.SetDegrees(degrees, 4));

  CHECK_EQ(map.GetIndexOfDegree(Degree(0)), 0);
  CHECK_EQ(map.GetIndexOfDegree(Degree(2)), 1);
  CHECK_EQ(map.GetIndexOfDegree(Degree(9)), 0);
}

TEST_CASE("ScaleMap weighted selection respects weights and aliases") {
  ScaleMap map;
  const Degree degrees[] = {0, 2, 4, 5, 7};
  const float singlePickWeights[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f};
  int period = 99;

  CHECK(map.SetDegrees(degrees, 5));

  CHECK_EQ(map.GetWeightedDegree(0.0f, period, singlePickWeights, 5),
           Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedDegree(0.5f, period, singlePickWeights, 5),
           Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedMappedDegree(8, 0.75f, period, singlePickWeights, 5),
           Degree(4));
  CHECK_EQ(period, 1);

  CHECK_EQ(map.GetWeightedNote(0.25f, period, singlePickWeights, 5),
           Degree(4));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetWeightedNote(-6, 0.25f, period, singlePickWeights, 5),
           Degree(4));
  CHECK_EQ(period, -2);
}

TEST_CASE("ScaleMap zero-sum weights fall back to uniform selection") {
  ScaleMap map;
  const Degree degrees[] = {0, 2, 4, 5, 7};
  const float zeroWeights[] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  int period = 99;

  CHECK(map.SetDegrees(degrees, 5));

  CHECK_EQ(map.GetWeightedDegree(-1.0f, period, zeroWeights, 5), Degree(0));
  CHECK_EQ(map.GetWeightedDegree(0.40f, period, zeroWeights, 5), Degree(4));
  CHECK_EQ(map.GetWeightedDegree(1.0f, period, zeroWeights, 5), Degree(7));
}

TEST_CASE("ScaleMap truncates oversized degree arrays") {
  ScaleMap map;
  Degree degrees[MAX_DEGREES + 3];
  int period = 99;

  for (int i = 0; i < MAX_DEGREES + 3; ++i) {
    degrees[i] = static_cast<Degree>(i);
  }

  CHECK(map.SetDegrees(degrees, MAX_DEGREES + 3));
  CHECK_EQ(map.Count(), MAX_DEGREES);

  CHECK_EQ(map.GetMappedDegree(MAX_DEGREES - 1, period),
           static_cast<Degree>(MAX_DEGREES - 1));
  CHECK_EQ(period, 0);

  CHECK_EQ(map.GetMappedDegree(MAX_DEGREES, period), Degree(0));
  CHECK_EQ(period, 1);
}

TEST_MAIN();
