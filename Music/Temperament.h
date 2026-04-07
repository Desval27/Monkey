/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Temperament.h
 * @brief Musical Temperaments.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * Embedded-friendly temperament / scale / pitch support
 * Suitable for Daisy, Arduino, STM32, etc.
 * No dynamic allocation.
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include "MusicTypes.h"
#include "MusicHelpers.h"

namespace Music
{

////////////////////////////////////////////////////////////////////////////////
class LabelSet
{
  public:
    LabelSet() : _count(0), _labels(nullptr) {}

    bool attach(const char* const* labels, uint16_t count)
    {
        if(!labels || count == 0 || count > MAX_DEGREES)
            return false;

        _labels = labels;
        _count  = count;
        return true;
    }

    bool get(Degree index, char* out, size_t maxOut) const
    {
        if(!_labels || index >= _count || !out || maxOut == 0)
            return false;

        const char* src = TUNING_PGM_READ_PTR(&_labels[index]);
        tuning_copy_string(out, src, maxOut);
        return true;
    }

  private:
    uint16_t           _count;
    const char* const* _labels;
};

////////////////////////////////////////////////////////////////////////////////
class Temperament
{
  public:
    enum class Kind : uint8_t
    {
        RatioTable = 0,
        EqualDivision
    };

    Temperament()
    {
        clear();
        // Default to 12-EDO with octaves at each frequency doubling.
        makeEqualDivision();
    }

    void clear()
    {
        _kind             = Kind::RatioTable;
        _degreesPerPeriod = 0;
        _periodRatio      = 2.0f;

        // Defaults to no ratios
        for(size_t i = 0; i < MAX_DEGREES; ++i)
            _ratios[i] = 1.0f;

        rebuildPeriodMultipliers();
    }

    bool makeEqualDivision(uint16_t degreesPerPeriod = 12,
                           float    periodRatio      = 2.0f)
    {
        if(degreesPerPeriod == 0 || degreesPerPeriod > MAX_DEGREES
           || periodRatio <= 0.0f)
            return false;

        _kind             = Kind::EqualDivision;
        _degreesPerPeriod = degreesPerPeriod;
        _periodRatio      = periodRatio;

        for(uint16_t i = 0; i < _degreesPerPeriod; ++i)
            _ratios[i]
                = powf(_periodRatio, (float)i / (float)_degreesPerPeriod);

        rebuildPeriodMultipliers();
        return true;
    }

    bool makeRatioTable(const float* ratios,
                        uint16_t     count,
                        float        periodRatio = 2.0f)
    {
        if(!ratios || count == 0 || count > MAX_DEGREES || periodRatio <= 0.0f)
            return false;

        _kind             = Kind::RatioTable;
        _degreesPerPeriod = count;
        _periodRatio      = periodRatio;

        for(uint16_t i = 0; i < count; ++i)
            _ratios[i] = ratios[i];

        rebuildPeriodMultipliers();
        return true;
    }

    bool makeNormalizedRatioTable(const float* ratios,
                                  uint16_t     count,
                                  float        periodRatio = 2.0f)
    {
        if(!ratios || count == 0 || count > MAX_DEGREES || periodRatio <= 1.0f)
            return false;

        _kind             = Kind::RatioTable;
        _degreesPerPeriod = count;
        _periodRatio      = periodRatio;

        for(uint16_t i = 0; i < count; ++i)
        {
            float r = ratios[i];
            if(r <= 0.0f)
                return false;

            while(r < 1.0f)
                r *= _periodRatio;
            while(r >= _periodRatio)
                r /= _periodRatio;

            _ratios[i] = r;
        }

        sortRatios();
        rebuildPeriodMultipliers();
        return true;
    }

    bool attachNoteLabels(const char* const* labels, uint16_t count)
    { return _noteLabels.attach(labels, count); }

    bool attachIntervalLabels(const char* const* labels, uint16_t count)
    { return _intervalLabels.attach(labels, count); }

    // -------------------------------------------------------------------------
    bool getNoteLabel(Degree degree, char* out, size_t maxOut) const
    {
        if(_degreesPerPeriod == 0)
            return false;
        return _noteLabels.get(
            static_cast<Degree>(wrap(static_cast<int>(degree),
                                     static_cast<int>(_degreesPerPeriod))),
            out,
            maxOut);
    }

    // -------------------------------------------------------------------------
    bool getIntervalLabel(Degree degree, char* out, size_t maxOut) const
    {
        if(_degreesPerPeriod == 0)
            return false;
        return _intervalLabels.get(
            static_cast<Degree>(wrap(static_cast<int>(degree),
                                     static_cast<int>(_degreesPerPeriod))),
            out,
            maxOut);
    }

    uint16_t degreesPerPeriod() const { return _degreesPerPeriod; }
    float    periodRatio() const { return _periodRatio; }

    float ratioAt(Degree degree) const
    {
        if(_degreesPerPeriod == 0)
            return 1.0f;

        int idx = wrap(static_cast<int>(degree),
                       static_cast<int>(_degreesPerPeriod));
        return _ratios[idx];
    }

    float periodMultiplier(Period period) const
    {
        if(period >= MIN_PERIOD && period <= MAX_PERIOD)
            return _periodMultipliers[period - MIN_PERIOD];

        return fastPow(_periodRatio, period);
    }

    float ratioAt(int degree, Period period) const
    { return ratioAt(degree) * periodMultiplier(period); }

    float frequencyFromRoot(float  rootHz,
                            Degree degree,
                            Period period    = 0,
                            float  fineCents = 0.0f) const
    {
        float ratio = ratioAt(degree, period);
        if(fineCents != 0.0f)
            ratio *= centsToRatio(fineCents);
        return rootHz * ratio;
    }

    float frequencyFromC0(Degree degree,
                          Period period    = 0,
                          float  fineCents = 0.0F) const
    { return frequencyFromRoot(FREQ_C0, degree, period, fineCents); }

    float frequencyFromReference(const TemperedPitch&   pitch,
                                 const TuningReference& ref) const
    {
        int relDegree = pitch.degree - ref.referenceDegree;
        int relPeriod = pitch.period - ref.referencePeriod;

        float ratio = ratioAt(relDegree, relPeriod);

        if(pitch.fineCents != 0.0f)
            ratio *= centsToRatio(pitch.fineCents);

        return ref.referenceHz * ratio;
    }

    bool findNearestDegreeForRatio(float   targetRatio,
                                   Degree& outDegree,
                                   float&  outErrorCents) const
    {
        if(_degreesPerPeriod == 0 || targetRatio <= 0.0f)
            return false;

        while(targetRatio < 1.0f)
            targetRatio *= _periodRatio;
        while(targetRatio >= _periodRatio)
            targetRatio /= _periodRatio;

        Degree best    = 0;
        float  bestErr = 1e30f;

        for(uint16_t i = 0; i < _degreesPerPeriod; ++i)
        {
            float err    = ratioToCents(targetRatio / _ratios[i]);
            float absErr = fabsf(err);
            if(absErr < bestErr)
            {
                bestErr       = absErr;
                best          = (Degree)i;
                outErrorCents = err;
            }
        }

        outDegree = best;
        return true;
    }

  private:
    const float FREQ_C0 = 16.35f;

    Kind     _kind;
    uint16_t _degreesPerPeriod;
    float    _periodRatio;
    float    _ratios[MAX_DEGREES];
    float    _periodMultipliers[(MAX_PERIOD - MIN_PERIOD) + 1];
    LabelSet _noteLabels;
    LabelSet _intervalLabels;

    void rebuildPeriodMultipliers()
    {
        for(Period p = MIN_PERIOD; p <= MAX_PERIOD; ++p)
            _periodMultipliers[p - MIN_PERIOD] = fastPow(_periodRatio, p);
    }

    void sortRatios()
    {
        for(uint16_t i = 1; i < _degreesPerPeriod; ++i)
        {
            float key = _ratios[i];
            int   j   = (int)i - 1;

            while(j >= 0 && _ratios[j] > key)
            {
                _ratios[j + 1] = _ratios[j];
                --j;
            }

            _ratios[j + 1] = key;
        }
    }
};


} // namespace Music
