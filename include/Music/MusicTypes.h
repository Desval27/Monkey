/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file MusicTypes.h
 * @brief Music Data Type Definitions.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <cstdint>

namespace Music
{

    ///////////////////////////////////////////////////////////////////////////////
    // Core Music Constants
    ///////////////////////////////////////////////////////////////////////////////
    constexpr float BASE_HZ = 440.0f; // A4 = 440Hz

    constexpr int MAX_DEGREES = 64;
    constexpr int MIN_PERIOD = -8;
    constexpr int MAX_PERIOD = 8;

    ///////////////////////////////////////////////////////////////////////////////
    // Core Music Types
    ///////////////////////////////////////////////////////////////////////////////
    using Note = int8_t;
    using Degree = int8_t;
    using Period = int8_t;

    /**
     * @brief Represents a scale degree within the musical context.
     *
     */
    enum class ScaleDegree : int
    {
        // Major Scale Degrees
        I,     // Major First
        ii,    // Minor Second
        iii,   // Minor Third
        IV,    // Major Fourth
        V,     // Major Fifth
        vi,    // Minor Sixth
        vii_o, // Diminished Seventh

        // Minor Scale Degrees
        i,    // Minor First
        ii_o, // Diminished Second
        III,  // Major Third
        iv,   // Minor Fourth
        v,    // Minor Fifth
        VI,   // Major Sixth
        VII,  // Major Seventh
    };

    ///////////////////////////////////////////////////////////////////////////////
    enum class HarmonicMode : uint8_t
    {
        Major,
        Minor,
    };

    ///////////////////////////////////////////////////////////////////////////////
    enum class Articulation : uint8_t
    {
        Staccato,
        Normal,
        Legato,
    };

    ///////////////////////////////////////////////////////////////////////////////
    struct PitchRef
    {
        float baseHz = BASE_HZ;
        Degree baseDegree = 0;
        Period basePeriod = 0;
    };

    /**
     * @brief 
     * 
     */
    struct TemperedPitch
    {
        Degree degree;
        Period period;
        float fineCents;

        TemperedPitch() : degree(0), period(0), fineCents(0.0f) {}

        TemperedPitch(Degree d, Period p, float fc = 0.0f)
            : degree(d), period(p), fineCents(fc)
        {
        }
    };

    /**
     * @brief Represents a tuning reference within the musical context.
     *
     */
    struct TuningReference
    {
        float referenceHz;
        Degree referenceDegree;
        Period referencePeriod;

        /**
         * @brief Construct a new Tuning Reference object
         *
         */
        TuningReference()
            : referenceHz(BASE_HZ), referenceDegree(0), referencePeriod(0)
        {
        }

        /**
         * @brief Construct a new Tuning Reference object with specific values
         *
         * @param hz
         * @param d
         * @param p
         */
        TuningReference(float hz, Degree d, Period p)
            : referenceHz(hz), referenceDegree(d), referencePeriod(p)
        {
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct TemperamentTable
    {
        int degreesInPeriod;
        const char *const *noteLabels;
        const char *const *intervalLabels;
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct ScaleTable
    {
        int degreesInPeriod;
        int degreesInScale;
        const char *name;
        HarmonicMode harmonicMode;
        const Degree *degrees;
        size_t degreeCount;

        bool IsMinor() const
        {
            return harmonicMode == HarmonicMode::Minor;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct WeightTable
    {
        int degreesInScale;
        const char *name;
        const float *weights;
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct NoteEventScore
    {
        int overall = 0;
        int density = 0;
        int rests = 0;
        int cadence = 0;
        int repetition = 0;
        int contour = 0;
        int phrase = 0;
        int rhythm = 0;
    };
    
} // namespace Music