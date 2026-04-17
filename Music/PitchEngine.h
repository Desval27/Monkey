/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file PitchEngine.h
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/Temperament.h>

namespace Music {
/**
 * @brief Quantized pitch helper using temperament + scale map
 *
 */
class PitchEngine {
 public:
  enum class RandomShape : uint8_t { Uniform = 0, BiasedLow, BiasedHigh };

  PitchEngine() : _t(nullptr), _s(nullptr), _rootHz(BASE_HZ) {}
  PitchEngine(const Temperament *t, const ScaleMap *s, float rootHz)
      : _t(t), _s(s), _rootHz(rootHz) {}

  void SetTemperament(const Temperament *t) { _t = t; }
  void SetScaleMap(const ScaleMap *s) { _s = s; }
  void SetRootHz(float rootHz) { _rootHz = rootHz; }

  /**
   * @brief
   *
   * @param scaleIndex
   * @param extraPeriod
   * @param fineCents
   * @return float
   */
  float FrequencyFromScaleIndex(int scaleIndex, int extraPeriod = 0,
                                float fineCents = 0.0f) const {
    if (!_t) return _rootHz;

    if (!_s || _s->Count() == 0)
      return _t->FrequencyFromRoot(_rootHz, scaleIndex, extraPeriod, fineCents);

    int scalePeriod = 0;
    Degree degree = _s->GetMappedDegree(scaleIndex, scalePeriod);
    return _t->FrequencyFromRoot(_rootHz, degree, scalePeriod + extraPeriod,
                                 fineCents);
  }

  /**
   * @brief
   *
   * @param scaleIndex
   * @param unitRandom
   * @param weights
   * @param weightCount
   * @param extraPeriod
   * @param fineCents
   * @return float
   */
  float FrequencyFromWeightedScaleIndex(int scaleIndex, float unitRandom,
                                        const float weights[], int weightCount,
                                        int extraPeriod = 0,
                                        float fineCents = 0.0f) const {
    if (!_t) return _rootHz;

    if (!_s || _s->Count() == 0)
      return _t->FrequencyFromRoot(_rootHz, scaleIndex, extraPeriod, fineCents);

    int scalePeriod;
    Degree degree = _s->GetWeightedMappedDegree(
        scaleIndex, unitRandom, scalePeriod, weights, weightCount);
    return _t->FrequencyFromRoot(_rootHz, degree, scalePeriod + extraPeriod,
                                 fineCents);
  }

  /**
   * @brief
   *
   * @param scaleIndex
   * @param unitRandom
   * @param shape
   * @param weights
   * @param weightCount
   * @param extraPeriod
   * @param fineCents
   * @return float
   */
  float FrequencyFromWeightedScaleIndex(int scaleIndex, float unitRandom,
                                        RandomShape shape,
                                        const float weights[], int weightCount,
                                        int extraPeriod = 0,
                                        float fineCents = 0.0f) const {
    return FrequencyFromWeightedScaleIndex(
        scaleIndex, ShapeUnitRandom(unitRandom, shape), weights, weightCount,
        extraPeriod, fineCents);
  }

  /**
   * @brief
   *
   * @param unitRandom
   * @param shape
   * @return float
   */
  static float ShapeUnitRandom(float unitRandom, RandomShape shape) {
    float r = ClampUnit(unitRandom);
    switch (shape) {
      case RandomShape::BiasedLow:
        return r * r;
      case RandomShape::BiasedHigh:
        return 1.0f - ((1.0f - r) * (1.0f - r));
      case RandomShape::Uniform:
      default:
        return r;
    }
  }

  /**
   * @brief
   *
   * @param pitch
   * @return float
   */
  float Frequency(const TemperedPitch &pitch) const {
    if (!_t) return _rootHz;

    return _t->FrequencyFromRoot(_rootHz, pitch.degree, pitch.period,
                                 pitch.fineCents);
  }

 private:
  const Temperament *_t;
  const ScaleMap *_s;
  float _rootHz;

  /**
   * @brief
   *
   * @param v
   * @return float
   */
  static float ClampUnit(float v) {
    if (v < 0.0f) return 0.0f;
    if (v >= 1.0f) return 0.999999f;
    return v;
  }
};

}  // namespace Music