/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file scale_map.hpp
 * @brief Scale-degree mapping helpers for indexed melodic and harmonic access.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <array>
#include <cstddef>

#include <Music/music_types.hpp>
#include <monkey.hpp>

namespace music {

///////////////////////////////////////////////////////////////////////////////
/// @brief Maps integer indices to scale degrees with optional weighted
/// selection.
///
/// The map stores a repeating list of scale degrees. Out-of-range indices are
/// wrapped back into the stored degree list, and the number of wraps is
/// returned as a period offset.
///
/// @tparam SCALE_DEGREES
template<std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES>
class ScaleMap
{
public:
  /////////////////////////////////////////////////////////////////////////////
  /// @brief Constructs an empty scale map.
  ScaleMap() { degrees_.fill(0); }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param degrees
  void set_degrees(const DegreeMap<SCALE_DEGREES>& degrees)
  {
    degrees_ = degrees;
    harmonic_mode_ = infer_harmonic_mode(degrees_);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param degrees
  /// @param harmonic_mode
  void set_degrees(const DegreeMap<SCALE_DEGREES>& degrees,
                   HarmonicMode harmonic_mode)
  {
    degrees_ = degrees;
    harmonic_mode_ = harmonic_mode;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam TEMPERAMENT_DEGREES
  /// @param scale
  template<std::size_t TEMPERAMENT_DEGREES>
  void set_scale(const ScaleTable<TEMPERAMENT_DEGREES, SCALE_DEGREES>& scale)
  {
    set_degrees(scale.degrees, scale.harmonic_mode);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param harmonic_mode
  void set_harmonic_mode(HarmonicMode harmonic_mode)
  {
    harmonic_mode_ = harmonic_mode;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Returns the number of degrees currently stored in the map.
  /// @return Number of mapped degrees.
  [[nodiscard]] std::size_t size() const { return degrees_.size(); }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] HarmonicMode get_harmonic_mode() const
  {
    return harmonic_mode_;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Resolves an index into a mapped degree and period offset.
  ///
  /// The index wraps across the stored degree list. The number of completed
  /// wraps is returned through @p out_period_offset.
  ///
  /// @param index Index to resolve.
  /// @param out_period_offset Receives the wrapped period offset.
  /// @return Mapped scale degree, or `0` when the map is empty.
  Degree get_mapped_degree(std::size_t index, int& out_period_offset) const
  {
    int wrapped = wrap(index, size());
    int cycles = (index - wrapped) / size();
    out_period_offset = cycles;
    return degrees_[wrapped];
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Finds the first stored index for a given degree.
  /// @param degree Degree to search for.
  /// @return Matching index, or `0` when the degree is not present.
  [[nodiscard]] std::size_t get_index_of_degree(Degree degree) const
  {
    for (std::size_t i = 0; i < degrees_.size(); i++) {
      if (degrees_[i] == degree) {
        return i;
      }
    }
    return 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Selects a stored degree using the supplied weights.
  ///
  /// This selection does not apply an index-based period shift.
  ///
  /// @param unit_random Unit random value in the range [0, 1).
  /// @param out_period_offset Receives the period offset, always `0` here.
  /// @param weights Per-degree selection weights.
  /// @return Selected degree, or `0` when the map is empty.
  Degree get_weighted_degree(float unit_random,
                             int& out_period_offset,
                             const WeightMap<SCALE_DEGREES>& weights) const
  {
    if (size() == 0) {
      out_period_offset = 0;
      return 0;
    }
    out_period_offset = 0;
    std::size_t idx = get_weighted_index(unit_random, weights);
    if (idx < size()) {
      return degrees_[idx];
    }
    return 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Alias for `get_weighted_degree()`.
  /// @param unit_random Unit random value in the range [0, 1).
  /// @param out_period_offset Receives the period offset.
  /// @param weights Per-degree selection weights.
  /// @return Selected degree, or `0` when the map is empty.
  Degree get_weighted_note(float unit_random,
                           int& out_period_offset,
                           const WeightMap<SCALE_DEGREES>& weights) const
  {
    return get_weighted_degree(unit_random, out_period_offset, weights);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Resolves an index to a period offset and selects a weighted degree.
  ///
  /// The period offset is derived from @p index, while the chosen degree comes
  /// from the weighted selection across the stored map entries.
  ///
  /// @param index Index used to determine the wrapped period offset.
  /// @param unit_random Unit random value in the range [0, 1).
  /// @param out_period_offset Receives the wrapped period offset.
  /// @param weights Per-degree selection weights.
  /// @return Selected degree, or `0` when the map is empty.
  Degree get_weighted_mapped_degree(
    std::size_t index,
    float unit_random,
    int& out_period_offset,
    const WeightMap<SCALE_DEGREES>& weights) const
  {
    int wrapped = wrap(index, size());
    int cycles = (index - wrapped) / size();
    out_period_offset = cycles;

    std::size_t idx = get_weighted_index(unit_random, weights);
    if (idx < size()) {
      return degrees_[idx];
    }
    return 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Alias for `get_weighted_mapped_degree()`.
  /// @param index Index used to determine the wrapped period offset.
  /// @param unit_random Unit random value in the range [0, 1).
  /// @param out_period_offset Receives the wrapped period offset.
  /// @param weights Per-degree selection weights.
  /// @return Selected degree, or `0` when the map is empty.
  Degree get_weighted_note(int index,
                           float unit_random,
                           int& out_period_offset,
                           const WeightMap<SCALE_DEGREES>& weights) const
  {
    return get_weighted_mapped_degree(
      index, unit_random, out_period_offset, weights);
  }

private:
  DegreeMap<SCALE_DEGREES> degrees_;
  HarmonicMode harmonic_mode_{ HarmonicMode::Major };

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param degrees
  /// @return
  static HarmonicMode infer_harmonic_mode(
    const DegreeMap<SCALE_DEGREES>& degrees)
  {
    if (SCALE_DEGREES < 3) {
      return HarmonicMode::Major;
    }

    const Degree third = static_cast<Degree>(degrees[2] - degrees[0]);
    const Degree fifth = (SCALE_DEGREES >= 5)
                           ? static_cast<Degree>(degrees[4] - degrees[0])
                           : static_cast<Degree>(third + third);

    return (third * 2 < fifth) ? HarmonicMode::Minor : HarmonicMode::Major;
  }

  /**
   * @brief
   *
   *
   * @param unit_random
   * @param weights
   * @return
   */

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Chooses a degree slot from the weight table.
  ///
  /// If the total weight is zero or negative, selection falls back to a
  /// uniform pick across the available entries.
  ///
  /// @param unit_random Unit random value in the range [0, 1).
  /// @param weights Per-degree selection weights.
  /// @return Selected array index.
  [[nodiscard]] std::size_t get_weighted_index(
    float unit_random,
    const WeightMap<SCALE_DEGREES>& weights) const
  {
    float sum = 0.0F;
    int this_count = min(size(), weights.size());
    if (this_count <= 0) {
      return 0;
    }

    for (int i = 0; i < this_count; ++i) {
      sum += weights[i];
    }

    if (sum <= 0.0F) {
      float r = clamp_unit(unit_random);
      int pick = static_cast<int>(r * static_cast<float>(this_count));
      if (pick >= this_count) {
        pick = this_count - 1;
      }
      return pick;
    }

    float target = clamp_unit(unit_random) * sum;
    float accum = 0.0F;
    for (int i = 0; i < this_count; ++i) {
      accum += weights[i];
      if (target < accum) {
        return i;
      }
    }

    return (this_count - 1);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief Clamps a value to the half-open unit interval [0, 1).
  ///
  /// Keeping the upper bound below `1.0f` prevents index calculations from
  /// stepping past the last valid element.
  ///
  /// @param v Input value to clamp.
  /// @return Clamped unit interval value.
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
