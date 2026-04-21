/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Chord.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief Chord definitions and voicing.
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <cstddef>

#include <Music/MusicTypes.h>
#include <Music/NoteValue.h>
#include <Music/ScaleMap.h>

namespace Music
{
    constexpr int SCALE_CHORD_COUNT = 7;

    enum class ChordExtension : uint16_t
    {

    };

    enum class ChordAlteration : uint16_t
    {

    };

    /**
     * @brief Represents a chord event in the musical context.
     * Includes the chord's root note, duration, extensions, alterations, and inversion.
     */
    struct ChordEvent
    {
        ChordEvent()
            : root(0),
              value(NoteValue::None),
              extensions(ChordExtension{}),
              alterations(ChordAlteration{}),
              inversion(0)
        {
        }

        ChordEvent(Note r, NoteValue v, ChordExtension ext = ChordExtension{},
                   ChordAlteration alt = ChordAlteration{}, int inv = 0)
            : root(r),
              value(v),
              extensions(ext),
              alterations(alt),
              inversion(inv)
        {
        }

        Note root;
        NoteValue value;
        ChordExtension extensions;
        ChordAlteration alterations;
        int inversion;

        // Rigged for the moment to just get chord tones for testing.
        size_t GetChordTones(const ScaleMap &scale, Note note[], size_t noteLen) const
        {
            // Rigged for testing
            size_t size = 0;
            int periodOffset;
            if (size < noteLen)
            {
                note[size] = root;
                uint16_t rootIdx = scale.GetIndexOfDegree(root);
                size++;
                if (size < noteLen)
                {
                    note[size] = scale.GetMappedDegree(rootIdx + 2, periodOffset);
                    size++;

                    if (size < noteLen)
                    {
                        note[size] = scale.GetMappedDegree(rootIdx + 4, periodOffset);
                        size++;
                    }
                }
            }
            return size;
        }
    };

    struct ChordTone
    {
        ScaleDegree degree; // temeprament or scale degree
        Period preferredPeriod;
        uint8_t flags; // optional: chord root, avoid doubling, tendency tone, etc.
    };

    struct ChordProbability
    {
        ScaleDegree fromDegree;
        float toDegree[SCALE_CHORD_COUNT];
    };

    static const ScaleDegree MAJOR_SCALE_DEGREES[SCALE_CHORD_COUNT] = {ScaleDegree::I,
                                                                       ScaleDegree::ii,
                                                                       ScaleDegree::iii,
                                                                       ScaleDegree::IV,
                                                                       ScaleDegree::V,
                                                                       ScaleDegree::vi,
                                                                       ScaleDegree::vii_o};

    static const ScaleDegree MINOR_SCALE_DEGREES[SCALE_CHORD_COUNT] = {ScaleDegree::i,
                                                                       ScaleDegree::ii_o,
                                                                       ScaleDegree::III,
                                                                       ScaleDegree::iv,
                                                                       ScaleDegree::v,
                                                                       ScaleDegree::VI,
                                                                       ScaleDegree::VII};

    static const float MAJOR_STARTING_CHORD_WEIGHTS[SCALE_CHORD_COUNT] = {4.0f, 0.50f, 0.25f, 1.25f, 2.50f, 1.50f, 0.10f};

    static const float MINOR_STARTING_CHORD_WEIGHTS[SCALE_CHORD_COUNT] = {4.0f, 0.20f, 1.25f, 1.00f, 1.50f, 1.25f, 1.00f};

    static const ChordProbability MajorScaleChordProbability[SCALE_CHORD_COUNT] = {
        //                    Technically I -> I is valid but I want to keep things interesting for now.
        //                         I      ii     iii    IV     V      vi     vii_0
        {ScaleDegree::I,
         /******/ {0.10f, 0.25f, 0.25f, 1.00f, 1.00f, 0.50f, 0.05f}},
        {ScaleDegree::ii,
         /*****/ {0.25f, 0.10f, 0.25f, 0.50f, 1.00f, 0.50f, 0.05f}},
        {ScaleDegree::iii,
         /****/ {0.25f, 0.25f, 0.10f, 0.50f, 0.25f, 0.75f, 0.05f}},
        {ScaleDegree::IV,
         /*****/ {0.50f, 0.50f, 0.25f, 0.10f, 1.00f, 0.25f, 0.05f}},
        {ScaleDegree::V,
         /******/ {1.00f, 0.25f, 0.25f, 0.50f, 0.10f, 0.50f, 0.05f}},
        {ScaleDegree::vi,
         /*****/ {0.25f, 1.00f, 0.50f, 0.50f, 1.00f, 0.10f, 0.05f}},
        {ScaleDegree::vii_o,
         /**/ {1.00f, 0.25f, 0.50f, 0.25f, 0.25f, 0.50f, 0.05f}},
    };

    static const ChordProbability MinorScaleChordProbability[SCALE_CHORD_COUNT] = {
        //                         i      ii_o   III    iv     v      VI     VII
        {ScaleDegree::i,
         /*******/ {0.10f, 0.15f, 0.75f, 1.00f, 1.00f, 0.75f, 0.75f}},
        {ScaleDegree::ii_o,
         /****/ {1.00f, 0.10f, 0.15f, 0.50f, 1.00f, 0.25f, 0.25f}},
        {ScaleDegree::III,
         /*****/ {0.75f, 0.15f, 0.10f, 0.75f, 0.50f, 1.00f, 0.75f}},
        {ScaleDegree::iv,
         /******/ {0.75f, 0.25f, 0.50f, 0.10f, 1.00f, 0.75f, 0.50f}},
        {ScaleDegree::v,
         /*******/ {1.00f, 0.25f, 0.25f, 0.75f, 0.10f, 0.50f, 0.75f}},
        {ScaleDegree::VI,
         /******/ {0.50f, 0.75f, 0.50f, 1.00f, 0.75f, 0.10f, 1.00f}},
        {ScaleDegree::VII,
         /*****/ {1.00f, 0.15f, 0.50f, 0.75f, 0.25f, 0.75f, 0.10f}},
    };

    static inline const ScaleDegree *ScaleDegreesForMode(HarmonicMode mode)
    {
        return (mode == HarmonicMode::Minor) ? MINOR_SCALE_DEGREES
                                             : MAJOR_SCALE_DEGREES;
    }

    static inline const ChordProbability *
    ChordProbabilityTableForMode(HarmonicMode mode)
    {
        return (mode == HarmonicMode::Minor) ? MinorScaleChordProbability
                                             : MajorScaleChordProbability;
    }

    static inline const float *StartingChordWeightsForMode(HarmonicMode mode)
    {
        return (mode == HarmonicMode::Minor) ? MINOR_STARTING_CHORD_WEIGHTS
                                             : MAJOR_STARTING_CHORD_WEIGHTS;
    }

    /**
     * @brief Gets the index of a scale degree within the context of a harmonic mode.
     *
     * @param degree The scale degree to find the index of.
     * @param mode The harmonic mode.
     * @return int The index of the scale degree, or -1 if not found.
     */
    static inline int ScaleDegreeIndex(ScaleDegree degree, HarmonicMode mode)
    {
        const ScaleDegree *degrees = ScaleDegreesForMode(mode);
        for (size_t i = 0; i < SCALE_CHORD_COUNT; ++i)
        {
            if (degrees[i] == degree)
                return static_cast<int>(i);
        }
        return -1;
    }

    static inline ScaleDegree ScaleDegreeFromIndex(int index, HarmonicMode mode)
    {
        if (index < 0)
            index = 0;

        const int wrapped = index % static_cast<int>(SCALE_CHORD_COUNT);
        return ScaleDegreesForMode(mode)[wrapped];
    }

    static inline Note ScaleDegreeToSemitone(ScaleDegree degree)
    {
        switch (degree)
        {
        case ScaleDegree::I:
            return Note_P1;
        case ScaleDegree::ii:
            return Note_M2;
        case ScaleDegree::iii:
            return Note_M3;
        case ScaleDegree::IV:
            return Note_P4;
        case ScaleDegree::V:
            return Note_P5;
        case ScaleDegree::vi:
            return Note_M6;
        case ScaleDegree::vii_o:
            return Note_M7;

        case ScaleDegree::i:
            return Note_P1;
        case ScaleDegree::ii_o:
            return Note_M2;
        case ScaleDegree::III:
            return Note_m3;
        case ScaleDegree::iv:
            return Note_P4;
        case ScaleDegree::v:
            return Note_P5;
        case ScaleDegree::VI:
            return Note_m6;
        case ScaleDegree::VII:
            return Note_m7;
        default:
            return Note_P1;
        }
    }

} // namespace Music
