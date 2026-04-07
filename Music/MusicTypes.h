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

namespace Music
{

///////////////////////////////////////////////////////////////////////////////
// Core Music Constants
///////////////////////////////////////////////////////////////////////////////
constexpr float BASE_HZ = 440.0f; // A4 = 440Hz

constexpr int MAX_DEGREES = 64;
constexpr int MIN_PERIOD  = -8;
constexpr int MAX_PERIOD  = 8;

///////////////////////////////////////////////////////////////////////////////
// Core Music Types
///////////////////////////////////////////////////////////////////////////////
using Note   = int8_t;
using Degree = int8_t;
using Period = int8_t;

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
struct PitchRef
{
    float  baseHz     = BASE_HZ;
    Degree baseDegree = 0;
    Period basePeriod = 0;
};

////////////////////////////////////////////////////////////////////////////////
struct TemperedPitch
{
    Degree degree;
    Period period;
    float  fineCents;

    TemperedPitch() : degree(0), period(0), fineCents(0.0f) {}
    TemperedPitch(Degree d, Period p, float fc = 0.0f)
    : degree(d), period(p), fineCents(fc)
    {
    }
};

////////////////////////////////////////////////////////////////////////////////
struct TuningReference
{
    float  referenceHz;
    Degree referenceDegree;
    Period referencePeriod;

    TuningReference()
    : referenceHz(BASE_HZ), referenceDegree(0), referencePeriod(0)
    {
    }

    TuningReference(float hz, Degree d, Period p)
    : referenceHz(hz), referenceDegree(d), referencePeriod(p)
    {
    }
};



} // namespace Music