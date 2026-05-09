#include <music/music.hpp>

#include "test_framework.hpp"

using music::Degree;
using music::PitchEngine;
using music::ScaleMap;
using music::Temperament;

static void
CheckNear(float actual, float expected, float tolerance = 0.001f)
{
  CHECK_IN_RANGE(actual, expected - tolerance, expected + tolerance);
}

TEST_CASE("PitchEngine returns root frequency without a temperament")
{
  PitchEngine<> engine;

  CheckNear(engine.FrequencyFromScaleIndex(7), music::BASE_HZ);
  CheckNear(engine.Frequency(music::TemperedPitch(7, 2, 50.0f)),
            music::BASE_HZ);

  engine.set_root_hz(123.45f);
  CheckNear(engine.FrequencyFromScaleIndex(7), 123.45f);
  CheckNear(engine.Frequency(music::TemperedPitch(7, 2, 50.0f)), 123.45f);
}

TEST_CASE("PitchEngine uses scale indices as temperament degrees without a "
          "scale map")
{
  Temperament<> temperament;
  PitchEngine<> engine(&temperament, nullptr, 110.0f);

  CheckNear(engine.FrequencyFromScaleIndex(7, 1),
            temperament.frequency_from_root(110.0f, Degree(7), 1));

  CheckNear(engine.FrequencyFromScaleIndex(4, -1, 25.0f),
            temperament.frequency_from_root(110.0f, Degree(4), -1, 25.0f));
}

TEST_CASE("PitchEngine maps scale indices through the active scale map")
{
  Temperament<> temperament;
  ScaleMap<music::PENTATONIC> scale;
  const music::DegreeMap<music::PENTATONIC> majorPentatonic = { 0, 2, 4, 7, 9 };
  PitchEngine<music::DEF_MAX_DEGREES, music::PENTATONIC> engine(
    &temperament, &scale, 220.0f);

  scale.set_degrees(majorPentatonic);

  CheckNear(engine.FrequencyFromScaleIndex(0),
            temperament.frequency_from_root(220.0f, Degree(0), 0));

  CheckNear(engine.FrequencyFromScaleIndex(4, 1),
            temperament.frequency_from_root(220.0f, Degree(9), 1));

  CheckNear(engine.FrequencyFromScaleIndex(5),
            temperament.frequency_from_root(220.0f, Degree(0), 1));

  CheckNear(engine.FrequencyFromScaleIndex(8, -1, -12.5f),
            temperament.frequency_from_root(220.0f, Degree(7), 0, -12.5f));
}

TEST_CASE("PitchEngine weighted lookup selects mapped degrees and keeps "
          "index period")
{
  Temperament<> temperament;
  ScaleMap<music::PENTATONIC> scale;
  const music::DegreeMap<music::PENTATONIC> majorPentatonic = { 0, 2, 4, 7, 9 };
  const music::WeightMap<music::PENTATONIC> thirdOnly = {
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f
  };
  PitchEngine<music::DEF_MAX_DEGREES, music::PENTATONIC> engine(
    &temperament, &scale, 220.0f);

  scale.set_degrees(majorPentatonic);

  CheckNear(engine.FrequencyFromWeightedScaleIndex(8, 0.75f, thirdOnly),
            temperament.frequency_from_root(220.0f, Degree(4), 1));

  CheckNear(
    engine.FrequencyFromWeightedScaleIndex(8, 0.75f, thirdOnly, 1, 10.0f),
    temperament.frequency_from_root(220.0f, Degree(4), 2, 10.0f));
}

TEST_CASE("PitchEngine random shaping biases weighted lookup")
{
  Temperament<> temperament;
  ScaleMap<music::PENTATONIC> scale;
  const music::DegreeMap<music::PENTATONIC> majorPentatonic = { 0, 2, 4, 7, 9 };
  const music::WeightMap<music::PENTATONIC> uniformWeights = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f
  };
  using PentatonicEngine =
    PitchEngine<music::DEF_MAX_DEGREES, music::PENTATONIC>;
  PentatonicEngine engine(&temperament, &scale, 220.0f);

  scale.set_degrees(majorPentatonic);

  CheckNear(
    engine.FrequencyFromWeightedScaleIndex(
      0, 0.5f, PentatonicEngine::RandomShape::BiasedLow, uniformWeights),
    temperament.frequency_from_root(220.0f, Degree(2), 0));

  CheckNear(
    engine.FrequencyFromWeightedScaleIndex(
      0, 0.5f, PentatonicEngine::RandomShape::BiasedHigh, uniformWeights),
    temperament.frequency_from_root(220.0f, Degree(7), 0));
}

TEST_CASE("PitchEngine shapes unit random values within half-open bounds")
{
  using RandomShape = PitchEngine<>::RandomShape;

  CheckNear(PitchEngine<>::ShapeUnitRandom(-0.5f, RandomShape::Uniform), 0.0f);
  CheckNear(PitchEngine<>::ShapeUnitRandom(1.0f, RandomShape::Uniform),
            0.999999f);
  CheckNear(PitchEngine<>::ShapeUnitRandom(0.5f, RandomShape::BiasedLow),
            0.25f);
  CheckNear(PitchEngine<>::ShapeUnitRandom(0.5f, RandomShape::BiasedHigh),
            0.75f);
}

TEST_CASE("PitchEngine converts tempered pitches with degree period and cents")
{
  Temperament<> temperament;
  PitchEngine<> engine(&temperament, nullptr, 330.0f);
  const music::TemperedPitch pitch(3, -1, 50.0f);

  CheckNear(engine.Frequency(pitch),
            temperament.frequency_from_root(330.0f, Degree(3), -1, 50.0f));
}

TEST_MAIN();
