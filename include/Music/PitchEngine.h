/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file PitchEngine.h
 * @brief Pitch quantization helpers backed by a temperament and optional scale
 * map.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/Music.h>

namespace Music {
/**
 * @brief Converts scale or tempered pitch inputs into frequencies.
 *
 * When a scale map is present, incoming scale indices are first mapped to scale
 * degrees and period offsets before the temperament converts them to Hz.
 */
template <std::size_t SCALE_MAP_DEGREES>
class PitchEngine {
 public:
  /**
   * @brief Shapes a unit random value before weighted note selection.
   */
  enum class RandomShape : uint8_t { Uniform = 0, BiasedLow, BiasedHigh };

  PitchEngine() : _t(nullptr), _s(nullptr), _rootHz(BASE_HZ) {}
  PitchEngine(const Temperament *t, const ScaleMap<SCALE_MAP_DEGREES> *s, float rootHz)
      : _t(t), _s(s), _rootHz(rootHz) {}

  void SetTemperament(const Temperament *t) { _t = t; }
  void SetScaleMap(const ScaleMap<SCALE_MAP_DEGREES> *s) { _s = s; }
  void SetRootHz(float rootHz) { _rootHz = rootHz; }

  /**
   * @brief Converts a scale index to a frequency in Hz.
   *
   * If no scale map is configured, the scale index is treated as a direct
   * temperament degree. Otherwise it is mapped through the active scale map,
   * and the mapped period offset is added to @p extraPeriod.
   *
   * @param scaleIndex Index within the current scale map.
   * @param extraPeriod Additional period offset applied after scale mapping.
   * @param fineCents Fine tuning offset in cents.
   * @return Frequency in Hz.
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
   * @brief Picks a weighted mapped degree and converts it to a frequency in Hz.
   *
   * The random input is expected in the half-open range [0, 1). If no scale map
   * is configured, @p scaleIndex is treated as a direct temperament degree.
   *
   * @param scaleIndex Base scale index used to determine the period offset.
   * @param unitRandom Unit random value used for weighted selection.
   * @param weights Per-degree weights used by the scale map.
   * @param weightCount Number of entries available in @p weights.
   * @param extraPeriod Additional period offset applied after scale mapping.
   * @param fineCents Fine tuning offset in cents.
   * @return Frequency in Hz.
   */
  template <std::size_t N>
  float FrequencyFromWeightedScaleIndex(int scaleIndex, float unitRandom,                                      
                                        const WeightMap<N>& weights,
                                        int extraPeriod = 0,
                                        float fineCents = 0.0f) const {
    if (!_t) return _rootHz;

    if (!_s || _s->Count() == 0)
      return _t->FrequencyFromRoot(_rootHz, scaleIndex, extraPeriod, fineCents);

    int scalePeriod;
    Degree degree = _s->GetWeightedMappedDegree(scaleIndex, unitRandom, scalePeriod, weights);
    return _t->FrequencyFromRoot(_rootHz, degree, scalePeriod + extraPeriod,
                                 fineCents);
  }

  /**
   * @brief Applies a random shaping curve, then performs weighted pitch lookup.
   *
   * @param scaleIndex Base scale index used to determine the period offset.
   * @param unitRandom Unit random value used for weighted selection.
   * @param shape Curve applied to @p unitRandom before selection.
   * @param weights Per-degree weights used by the scale map.
   * @param weightCount Number of entries available in @p weights.
   * @param extraPeriod Additional period offset applied after scale mapping.
   * @param fineCents Fine tuning offset in cents.
   * @return Frequency in Hz.
   */
  template <std::size_t N>
  float FrequencyFromWeightedScaleIndex(int scaleIndex, float unitRandom,
                                        RandomShape shape,
                                        const WeightMap<N>& weights,
                                        int extraPeriod = 0,
                                        float fineCents = 0.0f) const {
    return FrequencyFromWeightedScaleIndex(
        scaleIndex, ShapeUnitRandom(unitRandom, shape), weights, extraPeriod, fineCents);
  }

  /**
   * @brief Biases a unit random value toward low or high selections.
   *
   * The returned value is clamped to the half-open range [0, 1).
   *
   * @param unitRandom Source random value.
   * @param shape Bias curve to apply.
   * @return Shaped unit random value.
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
   * @brief Converts a fully specified tempered pitch to a frequency in Hz.
   *
   * @param pitch Tempered pitch expressed as degree, period, and fine cents.
   * @return Frequency in Hz.
   */
  float Frequency(const TemperedPitch &pitch) const {
    if (!_t) return _rootHz;

    return _t->FrequencyFromRoot(_rootHz, pitch.degree, pitch.period,
                                 pitch.fineCents);
  }

 private:
  const Temperament *_t;
  const ScaleMap<SCALE_MAP_DEGREES> *_s;
  float _rootHz;

  /**
   * @brief Clamps a value to the half-open unit interval [0, 1).
   *
   * Keeping the upper bound below 1.0 prevents weighted index calculations from
   * stepping one element past the end.
   *
   * @param v Input value to clamp.
   * @return Clamped unit interval value.
   */
  static float ClampUnit(float v) {
    if (v < 0.0f) return 0.0f;
    if (v >= 1.0f) return 0.999999f;
    return v;
  }
};

}  // namespace Music
