/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file ScaleMap.h
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

#include <Monkey.h>
#include <Music/MusicTypes.h>

namespace Music
{

  /**
   * @brief Maps integer indices to scale degrees with optional weighted
   * selection.
   * 
   * The map stores a repeating list of scale degrees. Out-of-range indices are
   * wrapped back into the stored degree list, and the number of wraps is
   * returned as a period offset.
   */
  template <std::size_t SCALE_DEGREES=DEF_SCALE_DEGREES>
  class ScaleMap
  {
  public:
    /**
     * @brief Constructs an empty scale map.
     */
    ScaleMap()
    {
      degrees_.fill(0);
    }

    void SetDegrees(const DegreeMap<SCALE_DEGREES>& degrees)
    {
      degrees_ = degrees;
    }

    /**
     * @brief Returns the number of degrees currently stored in the map.
     *
     * @return Number of mapped degrees.
     */
    std::size_t Count() const { return degrees_.size(); }

    /**
     * @brief Resolves an index into a mapped degree and period offset.
     *
     * The index wraps across the stored degree list. The number of completed
     * wraps is returned through @p outPeriodOffset.
     *
     * @param index Index to resolve.
     * @param outPeriodOffset Receives the wrapped period offset.
     * @return Mapped scale degree, or `0` when the map is empty.
     */
    Degree GetMappedDegree(std::size_t index, int &outPeriodOffset) const
    {
      int wrapped = wrap(index, Count());
      int cycles = (index - wrapped) / Count();
      outPeriodOffset = cycles;
      return degrees_[wrapped];
    }

    /**
     * @brief Finds the first stored index for a given degree.
     *
     * @param degree Degree to search for.
     * @return Matching index, or `0` when the degree is not present.
     */
    std::size_t GetIndexOfDegree(Degree degree) const
    {
      for (std::size_t i = 0; i < degrees_.size(); i++)
      {
        if (degrees_[i] == degree)
          return i;
      }
      return 0;

      // auto it = std::find(degrees_.begin(), degrees_.end(), degree);
      // if (it != degrees_.end()) 
      //   return *it;
      // return 0;
    }

    /**
     * @brief Selects a stored degree using the supplied weights.
     *
     * This selection does not apply an index-based period shift.
     *
     * @param unitRandom Unit random value in the range [0, 1).
     * @param outPeriodOffset Receives the period offset, always `0` here.
     * @param weights Per-degree selection weights.
     * @param weightCount Number of entries available in @p weights.
     * @return Selected degree, or `0` when the map is empty.
     */
    Degree GetWeightedDegree(float unitRandom, int &outPeriodOffset, const WeightMap<SCALE_DEGREES>& weights) const
    {
      if (Count() == 0)
      {
        outPeriodOffset = 0;
        return 0;
      }
      outPeriodOffset = 0;
      std::size_t idx = GetWeightedIndex(unitRandom, weights);
      if (idx < Count())
        return degrees_[idx];
      return 0;
    }

    /**
     * @brief Alias for `GetWeightedDegree()`.
     *
     * @param unitRandom Unit random value in the range [0, 1).
     * @param outPeriodOffset Receives the period offset.
     * @param weights Per-degree selection weights.
     * @param weightCount Number of entries available in @p weights.
     * @return Selected degree, or `0` when the map is empty.
     */
    Degree GetWeightedNote(float unitRandom, int &outPeriodOffset, const WeightMap<SCALE_DEGREES>& weights) const
    {
      return GetWeightedDegree(unitRandom, outPeriodOffset, weights);
    }

    /**
     * @brief Resolves an index to a period offset and selects a weighted degree.
     *
     * The period offset is derived from @p index, while the chosen degree comes
     * from the weighted selection across the stored map entries.
     *
     * @param index Index used to determine the wrapped period offset.
     * @param unitRandom Unit random value in the range [0, 1).
     * @param outPeriodOffset Receives the wrapped period offset.
     * @param weights Per-degree selection weights.
     * @param weightCount Number of entries available in @p weights.
     * @return Selected degree, or `0` when the map is empty.
     */
    Degree GetWeightedMappedDegree(std::size_t index, float unitRandom, int &outPeriodOffset, const WeightMap<SCALE_DEGREES>& weights) const
    {
      int wrapped = wrap(index, Count());
      int cycles = (index - wrapped) / Count();
      outPeriodOffset = cycles;

      std::size_t idx = GetWeightedIndex(unitRandom, weights);
      if (idx < Count())
        return degrees_[idx];
      return 0;
    }

    /**
     * @brief Alias for `GetWeightedMappedDegree()`.
     *
     * @param index Index used to determine the wrapped period offset.
     * @param unitRandom Unit random value in the range [0, 1).
     * @param outPeriodOffset Receives the wrapped period offset.
     * @param weights Per-degree selection weights.
     * @param weightCount Number of entries available in @p weights.
     * @return Selected degree, or `0` when the map is empty.
     */
    Degree GetWeightedNote(int index, float unitRandom, int &outPeriodOffset, const WeightMap<SCALE_DEGREES>& weights) const
    {
      return GetWeightedMappedDegree(index, unitRandom, outPeriodOffset, weights);
    }

  private:
    DegreeMap<SCALE_DEGREES> degrees_;

    /**
     * @brief Chooses a degree slot from the weight table.
     *
     * If the total weight is zero or negative, selection falls back to a
     * uniform pick across the available entries.
     *
     * @param unitRandom Unit random value in the range [0, 1).
     * @param weights Per-degree selection weights.
     * @param weightCount Number of entries available in @p weights.
     * @return Selected array index.
     */
    std::size_t GetWeightedIndex(float unitRandom, const WeightMap<SCALE_DEGREES>& weights) const
    {
      float sum = 0.0f;
      int thisCount = min(Count(), weights.size());
      if (thisCount <= 0)
        return 0;

      for (int i = 0; i < thisCount; ++i)
        sum += weights[i];

      if (sum <= 0.0f)
      {
        float r = ClampUnit(unitRandom);
        int pick = (int)(r * (float)thisCount);
        if (pick >= thisCount)
          pick = thisCount - 1;
        return pick;
      }

      float target = ClampUnit(unitRandom) * sum;
      float accum = 0.0f;
      for (int i = 0; i < thisCount; ++i)
      {
        accum += weights[i];
        if (target < accum)
          return i;
      }

      return (thisCount - 1);
    }

    /**
     * @brief Clamps a value to the half-open unit interval [0, 1).
     *
     * Keeping the upper bound below `1.0f` prevents index calculations from
     * stepping past the last valid element.
     *
     * @param v Input value to clamp.
     * @return Clamped unit interval value.
     */
    static float ClampUnit(float v)
    {
      if (v < 0.0f)
        return 0.0f;
      if (v >= 1.0f)
        return 0.999999f;
      return v;
    }
  };

} // namespace Music
