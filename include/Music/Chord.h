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

#include <cctype>
#include <cstddef>
#include <string>

#include <Music/MusicTypes.h>
#include <Music/NoteValue.h>
#include <Music/ScaleMap.h>

namespace Music
{
    class Temperament;

    constexpr int SCALE_CHORD_COUNT = 7;
    constexpr size_t DEFAULT_CHORD_TONE_COUNT = 3;

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
    template <std::size_t SCALE_MAP_INDEX>
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

        size_t GetChordTones(const ScaleMap<SCALE_MAP_INDEX> &scale, Note note[], size_t noteLen) const
        {
            return GetChordTones(scale, 12, note, noteLen);
        }

        size_t GetChordTones(const ScaleMap<SCALE_MAP_INDEX> &scale,
                             int degreesPerPeriod,
                             Note note[],
                             size_t noteLen) const
        {
            if (!note || noteLen == 0 || scale.Count() == 0 || degreesPerPeriod <= 0)
                return 0;

            const size_t rootIdx = scale.GetIndexOfDegree(root);
            if (rootIdx >= scale.Count())
                return 0;

            int rootPeriodOffset = 0;
            if (scale.GetMappedDegree(rootIdx, rootPeriodOffset) != root)
                return 0;

            const size_t toneCount =
                min(noteLen, min(static_cast<size_t>(scale.Count()),
                                 DEFAULT_CHORD_TONE_COUNT));

            for (size_t i = 0; i < toneCount; ++i)
            {
                int periodOffset = 0;
                const Note mapped =
                    static_cast<Note>(scale.GetMappedDegree(rootIdx + static_cast<int>(i * 2),
                                                            periodOffset));
                note[i] = static_cast<Note>(mapped + (periodOffset * degreesPerPeriod));
            }

            return toneCount;
        }

        std::string GetChordName(const ScaleMap<SCALE_MAP_INDEX> &scale,
                                 int degreesPerPeriod = 12) const
        {
            const size_t rootIdx = scale.GetIndexOfDegree(root);
            if (scale.Count() == 0 || rootIdx >= scale.Count())
                return "?";

            int periodOffset = 0;
            if (scale.GetMappedDegree(rootIdx, periodOffset) != root)
                return "?";

            Note tones[8];
            const size_t toneCount =
                GetChordTones(scale, degreesPerPeriod, tones, ArrayLen(tones));
            return BuildChordNameFromTones(scale, rootIdx, root, tones, toneCount,
                                           degreesPerPeriod);
        }

        std::string GetChordName(const ScaleMap<SCALE_MAP_INDEX> &scale,
                                 const Temperament &temperament) const;

    private:
        enum class ChordQuality : uint8_t
        {
            Unknown,
            Major,
            Minor,
            Diminished,
            Augmented,
            Suspended2,
            Suspended4,
            PowerChord,
        };

        static std::string BuildChordNameFromTones(const ScaleMap<SCALE_MAP_INDEX> &scale,
                                                   int scaleIndex,
                                                   Note chordRoot,
                                                   const Note tones[],
                                                   size_t toneCount,
                                                   int degreesPerPeriod)
        {
            std::string numeral = RomanNumeralForScaleIndex(scaleIndex);
            const ChordQuality quality =
                ClassifyChordQuality(scale, scaleIndex, chordRoot, tones, toneCount,
                                     degreesPerPeriod);

            switch (quality)
            {
            default:
                return BuildChordNameFromQuality(scaleIndex, quality);
            }
        }

        static ChordQuality ClassifyChordQuality(const ScaleMap<SCALE_MAP_INDEX> &scale,
                                                 int scaleIndex,
                                                 Note chordRoot,
                                                 const Note tones[],
                                                 size_t toneCount,
                                                 int degreesPerPeriod)
        {
            if (!tones || toneCount < 3 || degreesPerPeriod <= 0 || scale.Count() < 3)
                return ChordQuality::Unknown;

            int thirdIntervals[SCALE_CHORD_COUNT] = {};
            int fifthIntervals[SCALE_CHORD_COUNT] = {};
            size_t thirdCount = 0;
            size_t fifthCount = 0;

            for (size_t i = 0; i < scale.Count() && i < SCALE_CHORD_COUNT; ++i)
            {
                thirdIntervals[thirdCount++] =
                    GetScaleSpanInterval(scale, i, 2, degreesPerPeriod);
                fifthIntervals[fifthCount++] =
                    GetScaleSpanInterval(scale, i, 4, degreesPerPeriod);
            }

            const int minorThird = MinValue(thirdIntervals, thirdCount);
            const int majorThird = MaxValue(thirdIntervals, thirdCount);

            const int triadThird =
                wrap(static_cast<int>(tones[1] - chordRoot), degreesPerPeriod);
            const int triadFifth =
                wrap(static_cast<int>(tones[2] - chordRoot), degreesPerPeriod);

            const int diminishedFifth =
                GreatestValueLessThan(fifthIntervals, fifthCount, triadFifth);
            const int augmentedFifth =
                LeastValueGreaterThan(fifthIntervals, fifthCount, triadFifth);

            int perfectFifth = triadFifth;
            bool foundPerfectFifth = false;
            for (size_t i = 0; i < fifthCount; ++i)
            {
                const int candidate = fifthIntervals[i];
                if (candidate > diminishedFifth &&
                    (!foundPerfectFifth || candidate < perfectFifth))
                {
                    perfectFifth = candidate;
                    foundPerfectFifth = true;
                }
            }

            if (triadThird == majorThird && triadFifth == perfectFifth)
                return ChordQuality::Major;
            if (triadThird == minorThird && triadFifth == perfectFifth)
                return ChordQuality::Minor;
            if (triadThird == minorThird && triadFifth == diminishedFifth)
                return ChordQuality::Diminished;
            if (triadThird == majorThird && triadFifth == augmentedFifth)
                return ChordQuality::Augmented;

            const int upperThird =
                wrap(static_cast<int>(tones[2] - tones[1]), degreesPerPeriod);
            (void)scaleIndex;
            return ClassifyChordQualityFromThirdStack(triadThird, upperThird,
                                                      minorThird, majorThird);
        }

        static ChordQuality ClassifyChordQualityFromIntervalLabels(const char *thirdLabel,
                                                                   const char *fifthLabel)
        {
            if (!thirdLabel || !fifthLabel)
                return ChordQuality::Unknown;

            if (MatchesIntervalLabel(thirdLabel, "M3") &&
                MatchesIntervalLabel(fifthLabel, "P5"))
                return ChordQuality::Major;
            if (MatchesIntervalLabel(thirdLabel, "m3") &&
                MatchesIntervalLabel(fifthLabel, "P5"))
                return ChordQuality::Minor;
            if (MatchesIntervalLabel(thirdLabel, "m3") &&
                MatchesIntervalLabel(fifthLabel, "d5"))
                return ChordQuality::Diminished;
            if (MatchesIntervalLabel(thirdLabel, "M3") &&
                MatchesIntervalLabel(fifthLabel, "A5"))
                return ChordQuality::Augmented;
            if (MatchesIntervalLabel(thirdLabel, "M2") &&
                MatchesIntervalLabel(fifthLabel, "P5"))
                return ChordQuality::Suspended2;
            if (MatchesIntervalLabel(thirdLabel, "P4") &&
                MatchesIntervalLabel(fifthLabel, "P5"))
                return ChordQuality::Suspended4;
            return ChordQuality::Unknown;
        }

        static ChordQuality ClassifyChordQualityFromThirdStack(int lowerThird,
                                                               int upperThird,
                                                               int smallThird,
                                                               int largeThird)
        {
            if (lowerThird > upperThird)
                return ChordQuality::Major;
            if (lowerThird < upperThird)
                return ChordQuality::Minor;

            const int midpoint = smallThird + ((largeThird - smallThird) / 2);
            if (lowerThird <= midpoint)
                return ChordQuality::Diminished;
            return ChordQuality::Augmented;
        }

        static std::string BuildChordNameFromQuality(int scaleIndex,
                                                     ChordQuality quality)
        {
            std::string numeral = RomanNumeralForScaleIndex(scaleIndex);
            switch (quality)
            {
            case ChordQuality::Major:
                return numeral;
            case ChordQuality::Minor:
                LowercaseAscii(numeral);
                return numeral;
            case ChordQuality::Diminished:
                LowercaseAscii(numeral);
                return numeral + "_o";
            case ChordQuality::Augmented:
                return numeral + "+";
            case ChordQuality::Suspended2:
                return numeral + "sus2";
            case ChordQuality::Suspended4:
                return numeral + "sus4";
            case ChordQuality::PowerChord:
                return numeral + "5";
            case ChordQuality::Unknown:
            default:
                return numeral + "?";
            }
        }

        static int GetScaleSpanInterval(const ScaleMap<SCALE_MAP_INDEX> &scale,
                                        int rootIdx,
                                        int scaleSpan,
                                        int degreesPerPeriod)
        {
            int rootPeriodOffset = 0;
            int targetPeriodOffset = 0;
            const int rootDegree = scale.GetMappedDegree(rootIdx, rootPeriodOffset);
            const int targetDegree =
                scale.GetMappedDegree(rootIdx + scaleSpan, targetPeriodOffset);
            return (targetDegree + (targetPeriodOffset * degreesPerPeriod)) -
                   (rootDegree + (rootPeriodOffset * degreesPerPeriod));
        }

        static int MinValue(const int values[], size_t count)
        {
            if (!values || count == 0)
                return 0;

            int best = values[0];
            for (size_t i = 1; i < count; ++i)
            {
                if (values[i] < best)
                    best = values[i];
            }
            return best;
        }

        static int MaxValue(const int values[], size_t count)
        {
            if (!values || count == 0)
                return 0;

            int best = values[0];
            for (size_t i = 1; i < count; ++i)
            {
                if (values[i] > best)
                    best = values[i];
            }
            return best;
        }

        static int GreatestValueLessThan(const int values[], size_t count, int limit)
        {
            int best = 0;
            bool found = false;
            for (size_t i = 0; i < count; ++i)
            {
                if (values[i] < limit && (!found || values[i] > best))
                {
                    best = values[i];
                    found = true;
                }
            }
            return found ? best : limit;
        }

        static int LeastValueGreaterThan(const int values[], size_t count, int limit)
        {
            int best = 0;
            bool found = false;
            for (size_t i = 0; i < count; ++i)
            {
                if (values[i] > limit && (!found || values[i] < best))
                {
                    best = values[i];
                    found = true;
                }
            }
            return found ? best : limit;
        }

        static std::string RomanNumeralForScaleIndex(int scaleIndex)
        {
            switch (wrap(scaleIndex, SCALE_CHORD_COUNT))
            {
            case 0:
                return "I";
            case 1:
                return "II";
            case 2:
                return "III";
            case 3:
                return "IV";
            case 4:
                return "V";
            case 5:
                return "VI";
            case 6:
                return "VII";
            default:
                return "?";
            }
        }

        static void LowercaseAscii(std::string &text)
        {
            for (char &ch : text)
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }

        static bool MatchesIntervalLabel(const char *actual, const char *expected)
        {
            if (!actual || !expected)
                return false;

            size_t i = 0;
            while (expected[i] != '\0')
            {
                if (actual[i] != expected[i])
                    return false;
                ++i;
            }
            return actual[i] == '\0';
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
