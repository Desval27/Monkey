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

#include <Music/Music.h>

namespace Music
{

  /**
   * @brief
   *
   */
  class ScaleMap
  {
  public:
    /**
     * @brief
     *
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
     * @brief Set the Degrees object
     *
     * @param degrees
     * @param count
     * @return true
     * @return false
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
     * @brief
     *
     * @return int
     */
    int Count() const { return _count; }

    /**
     * @brief
     *
     * @param index
     * @param outPeriodOffset
     * @return Degree
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
     * @brief
     *
     * @param degree
     * @return int
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
     * @brief Get the Weighted Degree object
     *
     * @param unitRandom
     * @param outPeriodOffset
     * @param weights
     * @param weightCount
     * @return Degree
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
     * @brief Get the Weighted Note object
     *
     * @param unitRandom
     * @param outPeriodOffset
     * @param weights
     * @param weightCount
     * @return Degree
     */
    Degree GetWeightedNote(float unitRandom, int &outPeriodOffset,
                           const float weights[], int weightCount) const
    {
      return GetWeightedDegree(unitRandom, outPeriodOffset, weights, weightCount);
    }

    /**
     * @brief Get the Weighted Mapped Degree object
     *
     * @param index
     * @param unitRandom
     * @param outPeriodOffset
     * @param weights
     * @param weightCount
     * @return Degree
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
     * @brief
     *
     * @param index
     * @param unitRandom
     * @param outPeriodOffset
     * @param weights
     * @param weightCount
     * @return Degree
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
     * @brief Get the Weighted Index object
     *
     * @param unitRandom
     * @param weights
     * @param weightCount
     * @return int
     */
    int GetWeightedIndex(float unitRandom, const float weights[],
                         int weightCount) const
    {
      float sum = 0.0f;
      int thisCount = min(_count, weightCount);
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
        if (target <= accum)
          return i;
      }

      return (thisCount - 1);
    }

    /**
     * @brief
     *
     * @param v
     * @return float
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
