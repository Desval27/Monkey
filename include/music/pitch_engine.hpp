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

///////////////////////////////////////////////////////////////////////////////
/// @brief Converts scale or tempered pitch inputs into frequencies.
///
/// When a scale map is present, incoming scale indices are first mapped to
/// scale degrees and period offsets before the temperament converts them to Hz.
///
/// @tparam MAX_DEGREES Maximum number of degrees supported by the temperament.
/// @tparam SCALE_DEGREES Number of degrees supported by the scale map.
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES>
class PitchEngine
{
public:
  /////////////////////////////////////////////////////////////////////////////
  /// @brief Shapes a unit random value before weighted note selection.
  enum class RandomShape : uint8_t
  {
    Uniform = 0,
    BiasedLow,
    BiasedHigh
  };

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  PitchEngine()
    : temperament_(nullptr)
    , scale_map_(nullptr)
    , root_hz_(BASE_HZ)
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param temperament
  /// @param scale_map
  /// @param rootHz
  PitchEngine(const Temperament<MAX_DEGREES>* temperament,
              const ScaleMap<SCALE_DEGREES>* scale_map,
              float rootHz)
    : temperament_(temperament)
    , scale_map_(scale_map)
    , root_hz_(rootHz)
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
    scale_map_ = value;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param value
  void set_root_hz(float value) { root_hz_ = value; }

  /**
   * @brief Converts a scale index to a frequency in Hz.
   *
   * If no scale map is configured, the scale index is treated as a direct
   * temperament degree. Otherwise it is mapped through the active scale map,
   * and the mapped period offset is added to @p extra_period.
   *
   * @param scale_index Index within the current scale map.
   * @param extra_period Additional period offset applied after scale mapping.
   * @param fine_cents Fine tuning offset in cents.
   * @return get_frequency in Hz.
   */
  [[nodiscard]] float frequency_from_scale_index(int scale_index,
                                                 int extra_period = 0,
                                                 float fine_cents = 0.0F) const
  {
    if (temperament_ == nullptr) {
      return root_hz_;
    }

    if ((scale_map_ == nullptr) || scale_map_->size() == 0) {
      return temperament_->frequency_from_root(
        root_hz_, scale_index, extra_period, fine_cents);
    }

    int scale_period = 0;
    Degree degree = scale_map_->get_mapped_degree(scale_index, scale_period);
    return temperament_->frequency_from_root(
      root_hz_, degree, scale_period + extra_period, fine_cents);
  }

  /**
   * @brief Picks a weighted mapped degree and converts it to a frequency in
   * Hz.
   *
   * The random input is expected in the half-open range [0, 1). If no scale
   * map is configured, @p scale_index is treated as a direct temperament
   * degree.
   *
   * @param scale_index Base scale index used to determine the period offset.
   * @param unit_random Unit random value used for weighted selection.
   * @param weights Per-degree weights used by the scale map.
   * @param extra_period Additional period offset applied after scale mapping.
   * @param fine_cents Fine tuning offset in cents.
   * @return get_frequency in Hz.
   */
  template<std::size_t N>
  float frequency_from_weighted_scale_index(int scale_index,
                                            float unit_random,
                                            const WeightMap<N>& weights,
                                            int extra_period = 0,
                                            float fine_cents = 0.0F) const
  {
    if (!temperament_) {
      return root_hz_;
    }

    if (!scale_map_ || scale_map_->size() == 0) {
      return temperament_->frequency_from_root(
        root_hz_, scale_index, extra_period, fine_cents);
    }

    int scale_period;
    Degree degree = scale_map_->get_weighted_mapped_degree(
      scale_index, unit_random, scale_period, weights);
    return temperament_->frequency_from_root(
      root_hz_, degree, scale_period + extra_period, fine_cents);
  }

  /**
   * @brief Applies a random shaping curve, then performs weighted pitch
   * lookup.
   *
   * @param scale_index Base scale index used to determine the period offset.
   * @param unit_random Unit random value used for weighted selection.
   * @param shape Curve applied to @p unit_random before selection.
   * @param weights Per-degree weights used by the scale map.
   * @param extra_period Additional period offset applied after scale mapping.
   * @param fine_cents Fine tuning offset in cents.
   * @return get_frequency in Hz.
   */
  template<std::size_t N>
  float frequency_from_weighted_scale_index(int scale_index,
                                            float unit_random,
                                            RandomShape shape,
                                            const WeightMap<N>& weights,
                                            int extra_period = 0,
                                            float fine_cents = 0.0F) const
  {
    return frequency_from_weighted_scale_index(
      scale_index,
      shape_unit_random(unit_random, shape),
      weights,
      extra_period,
      fine_cents);
  }

  /**
   * @brief Biases a unit random value toward low or high selections.
   *
   * The returned value is clamped to the half-open range [0, 1).
   *
   * @param unit_random Source random value.
   * @param shape Bias curve to apply.
   * @return Shaped unit random value.
   */
  static float shape_unit_random(float unit_random, RandomShape shape)
  {
    float r = clamp_unit(unit_random);
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
   * @return get_frequency in Hz.
   */
  [[nodiscard]] float get_frequency(const TemperedPitch& pitch) const
  {
    if (temperament_ == nullptr) {
      return root_hz_;
    }

    return temperament_->frequency_from_root(
      root_hz_, pitch.degree, pitch.period, pitch.fine_cents);
  }

private:
  const Temperament<MAX_DEGREES>* temperament_;
  const ScaleMap<SCALE_DEGREES>* scale_map_;
  float root_hz_;

  /**
   * @brief Clamps a value to the half-open unit interval [0, 1).
   *
   * Keeping the upper bound below 1.0 prevents weighted index calculations
   * from stepping one element past the end.
   *
   * @param v Input value to clamp.
   * @return Clamped unit interval value.
   */
  static float clamp_unit(float v)
  {
    if (v < 0.0F) {
      return 0.0F;
    }
    if (v >= 1.0F) {
      return ALMOST_ONE;
    }
    return v;
  }
};

} // namespace Music
