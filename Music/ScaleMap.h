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

#include <algorithm>

#include "Monkey.h"
#include "Music/MusicTypes.h"

namespace Music
{
class ScaleMap
{
  public:
    ScaleMap() : _count(0)
    {
        for(int i = 0; i < MAX_DEGREES; ++i)
        {
            _degrees[i] = 0;
            // _weights[i] = 1.0f;
        }
    }

    bool setDegrees(const Degree* degrees, int count)
    {
        if(!degrees || count == 0)
            return false;

        _count = min(count, MAX_DEGREES);
        for(int i = 0; i < _count; ++i)
        {
            _degrees[i] = degrees[i];
            // _weights[i] = 1.0f;
        }

        return true;
    }

    // bool setWeightedDegrees(const Degree* degrees,
    //                         // const float*   weights,
    //                         uint16_t       count)
    // {
    //     if(!setDegrees(degrees, count))
    //         return false;

    //     return setWeights(weights, count);
    // }

    // bool setWeights(const float* weights, uint16_t count)
    // {
    //     if(!weights || _count == 0 || count != _count || count > MAX_DEGREES)
    //         return false;

    //     for(uint16_t i = 0; i < count; ++i)
    //         _weights[i] = (weights[i] > 0.0f) ? weights[i] : 0.0f;

    //     return true;
    // }

    int count() const { return _count; }

    Degree mappedDegree(int index, int& outPeriodOffset) const
    {
        if(_count == 0)
        {
            outPeriodOffset = 0;
            return 0;
        }

        int wrapped     = wrap(index, (int)_count);
        int cycles      = (index - wrapped) / (int)_count;
        outPeriodOffset = cycles;
        return _degrees[wrapped];
    }

    int indexOfDegree(Degree degree) const
    {
        for(int i = 0; i < _count; i++)
        {
            if(_degrees[i] == degree)
                return i;
        }
        return 0;
    }

    Degree weightedDegree(float unitRandom, int& outPeriodOffset, const float weights[], int weightCount) const
    {
        if(_count == 0)
        {
            outPeriodOffset = 0;
            return 0;
        }

        outPeriodOffset = 0;
        return _degrees[weightedIndex(unitRandom, weights, weightCount)];
    }

    Degree getWeightedNote(float unitRandom, int& outPeriodOffset, const float weights[], int weightCount) const
    { return weightedDegree(unitRandom, outPeriodOffset, weights, weightCount); }

    Degree weightedMappedDegree(int   index,
                                float unitRandom,
                                int&  outPeriodOffset,
                                const float weights[],
                                int weightCount) const
    {
        if(_count == 0)
        {
            outPeriodOffset = 0;
            return 0;
        }

        int wrapped     = wrap(index, (int)_count);
        int cycles      = (index - wrapped) / (int)_count;
        outPeriodOffset = cycles;

        return _degrees[weightedIndex(unitRandom, weights, weightCount)];
    }

    Degree
    getWeightedNote(int index, float unitRandom, int& outPeriodOffset, const float weights[], int weightCount) const
    { return weightedMappedDegree(index, unitRandom, outPeriodOffset, weights, weightCount); }

  private:
    int _count;
    Degree   _degrees[MAX_DEGREES];
    // float    _weights[MAX_DEGREES];

    int weightedIndex(float unitRandom, const float weights[], int weightCount) const
    {
        float sum = 0.0f;
        int thisCount = min(_count, weightCount);
        for(int i = 0; i < thisCount; ++i)
            sum += weights[i];

        if(sum <= 0.0f)
        {
            float  r    = clampUnit(unitRandom);
            int pick = (int)(r * (float)thisCount);
            if(pick >= thisCount)
                pick = thisCount - 1;
            return pick;
        }

        float target = clampUnit(unitRandom) * sum;
        float accum  = 0.0f;
        for(int i = 0; i < thisCount; ++i)
        {
            accum += weights[i];
            if(target <= accum)
                return i;
        }

        return (thisCount - 1);
    }

    static float clampUnit(float v)
    {
        if(v < 0.0f)
            return 0.0f;
        if(v >= 1.0f)
            return 0.999999f;
        return v;
    }
};


} // namespace Music
