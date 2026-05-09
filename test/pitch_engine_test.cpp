#include <Music/Music.h>

#include "TestFramework.h"

using Music::Degree;
using Music::PitchEngine;
using Music::ScaleMap;
using Music::Temperament;

static void
CheckNear(float actual, float expected, float tolerance = 0.001f)
{
  CHECK_IN_RANGE(actual, expected - tolerance, expected + tolerance);
}

TEST_CASE("PitchEngine returns root frequency without a temperament")
{
  PitchEngine<> engine;

  CheckNear(engine.FrequencyFromScaleIndex(7), Music::BASE_HZ);
  CheckNear(engine.Frequency(Music::TemperedPitch(7, 2, 50.0f)),
            Music::BASE_HZ);

  engine.SetRootHz(123.45f);
  CheckNear(engine.FrequencyFromScaleIndex(7), 123.45f);
  CheckNear(engine.Frequency(Music::TemperedPitch(7, 2, 50.0f)), 123.45f);
}

TEST_CASE("PitchEngine uses scale indices as temperament degrees without a "
          "scale map")
{
  Temperament<> temperament;
  PitchEngine<> engine(&temperament, nullptr, 110.0f);

  CheckNear(engine.FrequencyFromScaleIndex(7, 1),
            temperament.FrequencyFromRoot(110.0f, Degree(7), 1));

  CheckNear(engine.FrequencyFromScaleIndex(4, -1, 25.0f),
            temperament.FrequencyFromRoot(110.0f, Degree(4), -1, 25.0f));
}

TEST_CASE("PitchEngine maps scale indices through the active scale map")
{
  Temperament<> temperament;
  ScaleMap<Music::PENTATONIC> scale;
  const Music::DegreeMap<Music::PENTATONIC> majorPentatonic = { 0, 2, 4, 7, 9 };
  PitchEngine<Music::DEF_MAX_DEGREES, Music::PENTATONIC> engine(
    &temperament, &scale, 220.0f);

  scale.set_degrees(majorPentatonic);

  CheckNear(engine.FrequencyFromScaleIndex(0),
            temperament.FrequencyFromRoot(220.0f, Degree(0), 0));

  CheckNear(engine.FrequencyFromScaleIndex(4, 1),
            temperament.FrequencyFromRoot(220.0f, Degree(9), 1));

  CheckNear(engine.FrequencyFromScaleIndex(5),
            temperament.FrequencyFromRoot(220.0f, Degree(0), 1));

  CheckNear(engine.FrequencyFromScaleIndex(8, -1, -12.5f),
            temperament.FrequencyFromRoot(220.0f, Degree(7), 0, -12.5f));
}

TEST_CASE("PitchEngine weighted lookup selects mapped degrees and keeps "
          "index period")
{
  Temperament<> temperament;
  ScaleMap<Music::PENTATONIC> scale;
  const Music::DegreeMap<Music::PENTATONIC> majorPentatonic = { 0, 2, 4, 7, 9 };
  const Music::WeightMap<Music::PENTATONIC> thirdOnly = {
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f
  };
  PitchEngine<Music::DEF_MAX_DEGREES, Music::PENTATONIC> engine(
    &temperament, &scale, 220.0f);

  scale.set_degrees(majorPentatonic);

  CheckNear(engine.FrequencyFromWeightedScaleIndex(8, 0.75f, thirdOnly),
            temperament.FrequencyFromRoot(220.0f, Degree(4), 1));

  CheckNear(
    engine.FrequencyFromWeightedScaleIndex(8, 0.75f, thirdOnly, 1, 10.0f),
    temperament.FrequencyFromRoot(220.0f, Degree(4), 2, 10.0f));
}

TEST_CASE("PitchEngine random shaping biases weighted lookup")
{
  Temperament<> temperament;
  ScaleMap<Music::PENTATONIC> scale;
  const Music::DegreeMap<Music::PENTATONIC> majorPentatonic = { 0, 2, 4, 7, 9 };
  const Music::WeightMap<Music::PENTATONIC> uniformWeights = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.0f
  };
  using PentatonicEngine =
    PitchEngine<Music::DEF_MAX_DEGREES, Music::PENTATONIC>;
  PentatonicEngine engine(&temperament, &scale, 220.0f);

  scale.set_degrees(majorPentatonic);

  CheckNear(
    engine.FrequencyFromWeightedScaleIndex(
      0, 0.5f, PentatonicEngine::RandomShape::BiasedLow, uniformWeights),
    temperament.FrequencyFromRoot(220.0f, Degree(2), 0));

  CheckNear(
    engine.FrequencyFromWeightedScaleIndex(
      0, 0.5f, PentatonicEngine::RandomShape::BiasedHigh, uniformWeights),
    temperament.FrequencyFromRoot(220.0f, Degree(7), 0));
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
  const Music::TemperedPitch pitch(3, -1, 50.0f);

  CheckNear(engine.Frequency(pitch),
            temperament.FrequencyFromRoot(330.0f, Degree(3), -1, 50.0f));
}

TEST_MAIN();
