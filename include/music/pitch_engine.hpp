/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file pitch_engine.hpp
 * @brief pitch quantization helpers backed by a temperament and optional scale
 * map.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include <music/music_config.hpp>
#include <music/music_types.hpp>
#include <music/scale_map.hpp>
#include <music/temperament.hpp>

namespace music {
/**
 * @brief Converts scale or tempered pitch inputs into frequencies.
 *
 * When a scale map is present, incoming scale indices are first mapped to
 * scale degrees and period offsets before the temperament converts them to Hz.
 *
 * @tparam MAX_DEGREES Maximum number of degrees supported by the temperament.
 * @tparam SCALE_DEGREES Number of degrees supported by the scale map.
 */
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES>
class PitchEngine
{
public:
  /**
   * @brief Shapes a unit random value before weighted note selection.
   */
  enum class RandomShape : uint8_t
  {
    Uniform = 0,
    BiasedLow,
    BiasedHigh
  };

  PitchEngine()
    : temperament_(nullptr)
    , scaleMap_(nullptr)
    , rootHz_(BASE_HZ)
  {
  }
  PitchEngine(const Temperament<MAX_DEGREES>* temperament,
              const ScaleMap<SCALE_DEGREES>* scaleMap,
              float rootHz)
    : temperament_(temperament)
    , scaleMap_(scaleMap)
    , rootHz_(rootHz)
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Setters for the temperament, scale map, and root frequency.
  /// These allow dynamic reconfiguration of the pitch engine without
  /// reconstructing it. The temperament and scale map pointers are not
  /// owned by the pitch engine, so the caller must ensure they remain valid
  /// while in use.
  /// @param value
  void set_temperament(const Temperament<MAX_DEGREES>* value)
  {
    temperament_ = value;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param value
  void set_scale_map(const ScaleMap<SCALE_DEGREES>* value)
  {
    scaleMap_ = value;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param value
  void set_root_hz(float value) { rootHz_ = value; }

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
  [[nodiscard]] float FrequencyFromScaleIndex(int scaleIndex,
                                              int extraPeriod = 0,
                                              float fineCents = 0.0F) const
  {
    if (temperament_ == nullptr) {
      return rootHz_;
    }

    if ((scaleMap_ == nullptr) || scaleMap_->size() == 0) {
      return temperament_->frequency_from_root(
        rootHz_, scaleIndex, extraPeriod, fineCents);
    }

    int scalePeriod = 0;
    Degree degree = scaleMap_->get_mapped_degree(scaleIndex, scalePeriod);
    return temperament_->frequency_from_root(
      rootHz_, degree, scalePeriod + extraPeriod, fineCents);
  }

  /**
   * @brief Picks a weighted mapped degree and converts it to a frequency in
   * Hz.
   *
   * The random input is expected in the half-open range [0, 1). If no scale
   * map is configured, @p scaleIndex is treated as a direct temperament
   * degree.
   *
   * @param scaleIndex Base scale index used to determine the period offset.
   * @param unitRandom Unit random value used for weighted selection.
   * @param weights Per-degree weights used by the scale map.
   * @param extraPeriod Additional period offset applied after scale mapping.
   * @param fineCents Fine tuning offset in cents.
   * @return Frequency in Hz.
   */
  template<std::size_t N>
  float FrequencyFromWeightedScaleIndex(int scaleIndex,
                                        float unitRandom,
                                        const WeightMap<N>& weights,
                                        int extraPeriod = 0,
                                        float fineCents = 0.0F) const
  {
    if (!temperament_) {
      return rootHz_;
    }

    if (!scaleMap_ || scaleMap_->size() == 0) {
      return temperament_->frequency_from_root(
        rootHz_, scaleIndex, extraPeriod, fineCents);
    }

    int scalePeriod;
    Degree degree = scaleMap_->GetWeightedMappedDegree(
      scaleIndex, unitRandom, scalePeriod, weights);
    return temperament_->frequency_from_root(
      rootHz_, degree, scalePeriod + extraPeriod, fineCents);
  }

  /**
   * @brief Applies a random shaping curve, then performs weighted pitch
   * lookup.
   *
   * @param scaleIndex Base scale index used to determine the period offset.
   * @param unitRandom Unit random value used for weighted selection.
   * @param shape Curve applied to @p unitRandom before selection.
   * @param weights Per-degree weights used by the scale map.
   * @param extraPeriod Additional period offset applied after scale mapping.
   * @param fineCents Fine tuning offset in cents.
   * @return Frequency in Hz.
   */
  template<std::size_t N>
  float FrequencyFromWeightedScaleIndex(int scaleIndex,
                                        float unitRandom,
                                        RandomShape shape,
                                        const WeightMap<N>& weights,
                                        int extraPeriod = 0,
                                        float fineCents = 0.0F) const
  {
    return FrequencyFromWeightedScaleIndex(scaleIndex,
                                           ShapeUnitRandom(unitRandom, shape),
                                           weights,
                                           extraPeriod,
                                           fineCents);
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
  static float ShapeUnitRandom(float unitRandom, RandomShape shape)
  {
    float r = ClampUnit(unitRandom);
    switch (shape) {
      case RandomShape::BiasedLow:
        return r * r;
      case RandomShape::BiasedHigh:
        return 1.0F - ((1.0F - r) * (1.0F - r));
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
  [[nodiscard]] float Frequency(const TemperedPitch& pitch) const
  {
    if (temperament_ == nullptr) {
      return rootHz_;
    }

    return temperament_->frequency_from_root(
      rootHz_, pitch.degree, pitch.period, pitch.fineCents);
  }

private:
  const Temperament<MAX_DEGREES>* temperament_;
  const ScaleMap<SCALE_DEGREES>* scaleMap_;
  float rootHz_;

  /**
   * @brief Clamps a value to the half-open unit interval [0, 1).
   *
   * Keeping the upper bound below 1.0 prevents weighted index calculations
   * from stepping one element past the end.
   *
   * @param v Input value to clamp.
   * @return Clamped unit interval value.
   */
  static float ClampUnit(float v)
  {
    if (v < 0.0F) {
      return 0.0F;
    }
    if (v >= 1.0F) {
      return 0.999999F;
    }
    return v;
  }
};

} // namespace Music
