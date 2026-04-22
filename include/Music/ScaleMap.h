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

#include <Music/Music.h>

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
  class ScaleMap
  {
  public:
    /**
     * @brief Constructs an empty scale map.
     */
    ScaleMap() : _count(0)
    {
      // Start with everything initialized to zeros.
      for (int i = 0; i < MAX_DEGREES; ++i)
      {
        _degrees[i] = 0;
      }
    }

    /**
     * @brief Replaces the stored degree list.
     *
     * Up to `MAX_DEGREES` entries are copied from @p degrees.
     *
     * @param degrees Source array of scale degrees.
     * @param count Number of source entries to copy.
     * @return `true` when at least one degree was copied, otherwise `false`.
     */
    bool SetDegrees(const Degree *degrees, int count)
    {
      if (!degrees || count == 0)
        return false;

      _count = min(count, MAX_DEGREES);
      for (int i = 0; i < _count; ++i)
      {
        _degrees[i] = degrees[i];
      }
      return true;
    }

    /**
     * @brief Returns the number of degrees currently stored in the map.
     *
     * @return Number of mapped degrees.
     */
    int Count() const { return _count; }

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
    Degree GetMappedDegree(int index, int &outPeriodOffset) const
    {
      if (_count == 0)
      {
        outPeriodOffset = 0;
        return 0;
      }

      int wrapped = wrap(index, _count);
      int cycles = (index - wrapped) / _count;
      outPeriodOffset = cycles;
      return _degrees[wrapped];
    }

    /**
     * @brief Finds the first stored index for a given degree.
     *
     * @param degree Degree to search for.
     * @return Matching index, or `0` when the degree is not present.
     */
    int GetIndexOfDegree(Degree degree) const
    {
      for (int i = 0; i < _count; i++)
      {
        if (_degrees[i] == degree)
          return i;
      }
      return 0;
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
    Degree GetWeightedDegree(float unitRandom, int &outPeriodOffset,
                             const float weights[], int weightCount) const
    {
      if (_count == 0)
      {
        outPeriodOffset = 0;
        return 0;
      }

      outPeriodOffset = 0;
      return _degrees[GetWeightedIndex(unitRandom, weights, weightCount)];
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
    Degree GetWeightedNote(float unitRandom, int &outPeriodOffset,
                           const float weights[], int weightCount) const
    {
      return GetWeightedDegree(unitRandom, outPeriodOffset, weights, weightCount);
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
    Degree GetWeightedMappedDegree(int index, float unitRandom, int &outPeriodOffset,
                                   const float weights[], int weightCount) const
    {
      if (_count == 0)
      {
        outPeriodOffset = 0;
        return 0;
      }

      int wrapped = wrap(index, _count);
      int cycles = (index - wrapped) / _count;
      outPeriodOffset = cycles;

      return _degrees[GetWeightedIndex(unitRandom, weights, weightCount)];
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
    Degree GetWeightedNote(int index, float unitRandom, int &outPeriodOffset,
                           const float weights[], int weightCount) const
    {
      return GetWeightedMappedDegree(index, unitRandom, outPeriodOffset, weights,
                                     weightCount);
    }

  private:
    int _count;
    Degree _degrees[MAX_DEGREES];

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
    int GetWeightedIndex(float unitRandom, const float weights[],
                         int weightCount) const
    {
      float sum = 0.0f;
      int thisCount = min(_count, weightCount);
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
