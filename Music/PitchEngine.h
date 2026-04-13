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

namespace Music
{
// --------------------------------------------------------
// Quantized pitch helper using temperament + scale map
// --------------------------------------------------------
class PitchEngine
{
  public:
    enum class RandomShape : uint8_t
    {
        Uniform = 0,
        BiasedLow,
        BiasedHigh
    };

    PitchEngine() : _temperament(nullptr), _scale(nullptr), _rootHz(BASE_HZ) {}

    void setTemperament(const Temperament* t) { _temperament = t; }
    void setScaleMap(const ScaleMap* s) { _scale = s; }
    void setRootHz(float hz) { _rootHz = hz; }

    float frequencyFromScaleIndex(int   scaleIndex,
                                  int   extraPeriod = 0,
                                  float fineCents   = 0.0f) const
    {
        if(!_temperament)
            return _rootHz;

        if(!_scale || _scale->count() == 0)
            return _temperament->frequencyFromRoot(
                _rootHz, scaleIndex, extraPeriod, fineCents);

        int    scalePeriod = 0;
        Degree degree      = _scale->mappedDegree(scaleIndex, scalePeriod);
        return _temperament->frequencyFromRoot(
            _rootHz, degree, scalePeriod + extraPeriod, fineCents);
    }

    float frequencyFromWeightedScaleIndex(int   scaleIndex,
                                          float unitRandom,
                                          const float weights[],
                                          int   weightCount,
                                          int   extraPeriod = 0,
                                          float fineCents   = 0.0f) const
    {
        if(!_temperament)
            return _rootHz;

        if(!_scale || _scale->count() == 0)
            return _temperament->frequencyFromRoot(
                _rootHz, scaleIndex, extraPeriod, fineCents);

        int    scalePeriod;
        Degree degree = _scale->weightedMappedDegree(
            scaleIndex, unitRandom, scalePeriod, weights, weightCount);
        return _temperament->frequencyFromRoot(
            _rootHz, degree, scalePeriod + extraPeriod, fineCents);
    }

    float frequencyFromWeightedScaleIndex(int         scaleIndex,
                                          float       unitRandom,
                                          RandomShape shape,
                                          const float weights[],
                                          int   weightCount,
                                          int   extraPeriod = 0,
                                          float fineCents   = 0.0f) const
    {
        return frequencyFromWeightedScaleIndex(
            scaleIndex,
            shapeUnitRandom(unitRandom, shape),
            weights, weightCount,
            extraPeriod,
            fineCents);
    }

    static float shapeUnitRandom(float unitRandom, RandomShape shape)
    {
        float r = clampUnit(unitRandom);
        switch(shape)
        {
            case RandomShape::BiasedLow: return r * r;
            case RandomShape::BiasedHigh:
                return 1.0f - ((1.0f - r) * (1.0f - r));
            case RandomShape::Uniform:
            default: return r;
        }
    }

    float frequency(const TemperedPitch& pitch) const
    {
        if(!_temperament)
            return _rootHz;

        return _temperament->frequencyFromRoot(
            _rootHz, pitch.degree, pitch.period, pitch.fineCents);
    }

  private:
    const Temperament* _temperament;
    const ScaleMap*    _scale;
    float              _rootHz;

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