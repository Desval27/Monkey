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

#include <array>
#include <cstdint>
#include <Music/MusicConfig.h>

namespace Music
{
    ///////////////////////////////////////////////////////////////////////////////
    // Core Music Constants
    ///////////////////////////////////////////////////////////////////////////////
    constexpr float BASE_HZ = 440.0f; // A4 = 440Hz

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

    /**
     * @brief Represents a type of musical scale based on the number of degrees it contains. This enumeration provides a way to classify scales according to their degree count, which can be used to identify common scale types such as pentatonic, hexatonic, heptatonic, etc., as well as more unusual scales with different numbers of degrees. The ScaleType can be used in conjunction with scale definitions to facilitate the generation and analysis of musical patterns and structures.
     */
    enum ScaleType
    {
        MONOTONIC = 1,
        DITONIC = 2,
        TRITONIC = 3,
        TETRATONIC = 4,
        PENTATONIC = 5,
        HEXATONIC = 6,
        HEPATONIC = 7,
        OCTATONIC = 8,
        NONATONIC = 9,
        DECATONIC = 11,
        UNDECATONIC = 11,
        DUODECATONIC = 12,
    };

    ///////////////////////////////////////////////////////////////////////////////
    // Convenient Type Aliases
    ///////////////////////////////////////////////////////////////////////////////
    template <std::size_t N = HEPATONIC>
    using DegreeMap = std::array<Degree, N>;
    template <std::size_t N = HEPATONIC>
    using WeightMap = std::array<float, N>;

    ///////////////////////////////////////////////////////////////////////////////
    // Common Music Data Structures
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * @brief Represents a reference pitch within the musical context, defined by a base frequency in Hertz (Hz), a reference scale degree, and a reference period. This structure serves as a fundamental reference point for calculating the frequencies of other pitches in the tuning system based on their relationships to this reference pitch in terms of scale degrees and periods. The base frequency provides the actual pitch in terms of sound, while the reference degree and period allow for understanding the pitch's position within the musical scale and its relationship to other pitches in terms of intervals and octave equivalence.
     */
    struct PitchRef
    {
        float baseHz = BASE_HZ; ///< The reference frequency in Hertz (Hz) for the tuning system, typically representing a standard pitch such as A4 = 440Hz.
        Degree baseDegree = 0;  ///< The reference scale degree associated with the reference frequency, indicating its position within the musical scale. This value can be used to determine the pitch's relationship to other pitches in terms of intervals and harmonic function.
        Period basePeriod = 0;  ///< The reference period associated with the reference frequency, indicating its position within the periodic structure of the tuning system. This value can be used to determine the pitch's relationship to other pitches in terms of octave equivalence and overall pitch height.
    };

    /**
     * @brief Represents a tempered pitch within the musical context, defined by its scale degree, period, and fine-tuning adjustment in cents. This structure allows for precise representation of pitches in various tuning systems, including equal temperament and microtonal tunings, by providing the necessary information to calculate the actual frequency of the pitch based on its relationship to a reference pitch and its position within the musical scale and periodic structure.
     */
    struct TemperedPitch
    {
        Degree degree;   ///< The scale degree of the pitch, indicating its position within the musical scale. This value can be used to determine the pitch's relationship to other pitches in terms of intervals and harmonic function.
        Period period;   ///< The period of the pitch, indicating its position within the periodic structure of the tuning system. This value can be used to determine the pitch's relationship to other pitches in terms of octave equivalence and overall pitch height.
        float fineCents; ///< The fine-tuning adjustment in cents, allowing for precise tuning of the pitch beyond the standard scale degrees and periods. This value can be used to achieve microtonal adjustments or to fine-tune pitches for specific musical contexts.

        TemperedPitch() : degree(0), period(0), fineCents(0.0f) {}

        TemperedPitch(Degree d, Period p, float fc = 0.0f)
            : degree(d), period(p), fineCents(fc)
        {
        }
    };

    /**
     * @brief Represents a tuning reference within the musical context.
     */
    struct TuningReference
    {
        float referenceHz;      ///< The reference frequency in Hertz (Hz) for the tuning system.
        Degree referenceDegree; ///< The reference scale degree associated with the reference frequency, indicating its position within the musical scale.
        Period referencePeriod; ///< The reference period associated with the reference frequency, indicating its position within the periodic structure of the tuning system.

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
         * @param hz The reference frequency in Hertz (Hz) for the tuning system.
         * @param d The reference scale degree associated with the reference frequency, indicating its position within the musical scale.
         * @param p The reference period associated with the reference frequency, indicating its position within the periodic structure of the tuning system.
         */
        TuningReference(float hz, Degree d, Period p)
            : referenceHz(hz), referenceDegree(d), referencePeriod(p)
        {
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Attack/Decay Envelope Parameters
    struct ADEnvelope
    {
        ADEnvelope()
        {
            attack = 0.0f;
            decay = 0.0f;
        }
        float attack;   ///< In Seconds
        float decay;    ///< In Seconds
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Attack/Release Envelope Parameters
    struct AREnvelope
    {
        AREnvelope()
        {
            attack = 0.0f;
            release = 0.0f;
        }
        float attack;   ///< In Seconds
        float release;  ///< In Seconds
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// @brief Attack/Decay/Sustain/Release Parameters
    struct ADSREnvelope
    {
        ADSREnvelope()
        {
            attack = 0.0f;
            decay = 0.0f;
            sustain = 0.0f;
            release = 0.0f;
        }
        float attack;   ///< In Seconds
        float decay;    ///< In Seconds
        float sustain;  ///< As 0.0 -> 1.0
        float release;  ///< In Seconds
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct EqualTemperamentTable
    {
        int degreesInPeriod;
        const char *name;
        const char *const *noteLabels;
        const char *const *intervalLabels;
    };

    ////////////////////////////////////////////////////////////////////////////////
    template <std::size_t DEGREES, std::size_t SCALE_TYPE>
    struct ScaleTable
    {
        const char *name;
        HarmonicMode harmonicMode;
        const DegreeMap<SCALE_TYPE> &degrees;

        bool IsMinor() const
        {
            return harmonicMode == HarmonicMode::Minor;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    template <std::size_t N>
    struct WeightTable
    {
        const char *name;           ///< The name of the weight table, providing a human-readable identifier for the set of weights it contains.
        const WeightMap<N> weights; ///< An array of weights corresponding to each degree in the scale, where each weight represents the relative importance or emphasis of that degree within the musical context. The weights can be used for various purposes, such as determining the likelihood of selecting a particular degree when generating melodies or harmonies, or for analyzing the structure of a musical piece based on the distribution of emphasis across its degrees.
    };

    ////////////////////////////////////////////////////////////////////////////////
    struct NoteEventScore
    {
        int overall = 0;    ///< Overall score representing the general quality or suitability of a note event within a musical context, potentially taking into account various factors such as harmonic compatibility, rhythmic placement, and melodic contour.
        int density = 0;    ///< Score representing the density of a note event, which could be based on factors such as the number of simultaneous notes (chord density) or the frequency of note occurrences within a given time frame (rhythmic density).
        int rests = 0;      ///< Score representing the presence and quality of rests in a musical sequence, which could be based on factors such as the duration and placement of rests, as well as their contribution to the overall phrasing and musical expression.
        int cadence = 0;    ///< Score representing the quality of cadences in a musical sequence, which could be based on factors such as the harmonic progression, rhythmic placement, and resolution of cadences, as well as their contribution to the overall structure and emotional impact of the music.
        int repetition = 0; ///< Score representing the presence and quality of repetition in a musical sequence, which could be based on factors such as the frequency and placement of repeated motifs or phrases, as well as their contribution to the overall coherence and memorability of the music.
        int contour = 0;    ///< Score representing the melodic contour of a musical sequence, which could be based on factors such as the direction and shape of melodic lines, as well as their contribution to the overall expressiveness and emotional impact of the music.
        int phrase = 0;     ///< Score representing the quality of phrasing in a musical sequence, which could be based on factors such as the grouping and articulation of notes into musical phrases, as well as their contribution to the overall flow and expressiveness of the music.
        int rhythm = 0;     ///< Score representing the quality of rhythm in a musical sequence, which could be based on factors such as the complexity and variety of rhythmic patterns, as well as their contribution to the overall energy and drive of the music.
    };

} // namespace Music
