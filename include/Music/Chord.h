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
#include <cstdint>

#include <Monkey.h>
#include <Music/MusicTypes.h>
#include <Music/NoteValue.h>
#include <Music/Temperament.h>
#include <Music/ScaleMap.h>

namespace Music
{
    constexpr int SCALE_CHORD_COUNT = 7;
    constexpr std::size_t DEFAULT_CHORD_TONE_COUNT = 3;

    enum class ChordExtension : uint16_t
    {
        None = 0,
        Sixth = 1 << 0,
        Seventh = 1 << 1,
        Ninth = 1 << 2,
        Eleventh = 1 << 3,
        Thirteenth = 1 << 4,
        Sus2 = 1 << 5,
        Sus4 = 1 << 6,
    };

    enum class ChordAlteration : uint16_t
    {
        None = 0,
        Flat5 = 1 << 0,
        Sharp5 = 1 << 1,
        Flat9 = 1 << 2,
        Sharp9 = 1 << 3,
        Sharp11 = 1 << 4,
        Flat13 = 1 << 5,
    };

    static inline ChordExtension operator|(ChordExtension lhs, ChordExtension rhs)
    {
        return static_cast<ChordExtension>(
            static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
    }

    static inline ChordExtension operator&(ChordExtension lhs, ChordExtension rhs)
    {
        return static_cast<ChordExtension>(
            static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
    }

    static inline ChordExtension &operator|=(ChordExtension &lhs, ChordExtension rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    static inline ChordAlteration operator|(ChordAlteration lhs, ChordAlteration rhs)
    {
        return static_cast<ChordAlteration>(
            static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
    }

    static inline ChordAlteration operator&(ChordAlteration lhs, ChordAlteration rhs)
    {
        return static_cast<ChordAlteration>(
            static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
    }

    static inline ChordAlteration &operator|=(ChordAlteration &lhs, ChordAlteration rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    static inline bool HasFlag(ChordExtension value, ChordExtension flag)
    {
        return (static_cast<uint16_t>(value & flag) != 0);
    }

    static inline bool HasFlag(ChordAlteration value, ChordAlteration flag)
    {
        return (static_cast<uint16_t>(value & flag) != 0);
    }

    /**
     * @brief Represents a chord event in the musical context.
     * Includes the chord's root note, duration, extensions, alterations, and inversion.
     */
    template <
        std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
        std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES>
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

        std::size_t GetChordTones(const ScaleMap<SCALE_DEGREES> &scale, Note note[], std::size_t noteLen) const
        {
            return GetChordTones(scale, 12, note, noteLen);
        }

        std::size_t GetChordTones(const ScaleMap<SCALE_DEGREES> &scale,
                                  int degreesPerPeriod,
                                  Note note[],
                                  std::size_t noteLen) const
        {
            if (!note || noteLen == 0 || scale.Count() == 0 || degreesPerPeriod <= 0)
                return 0;

            const std::size_t rootIdx = scale.GetIndexOfDegree(root);
            if (rootIdx >= scale.Count())
                return 0;

            int rootPeriodOffset = 0;
            if (scale.GetMappedDegree(rootIdx, rootPeriodOffset) != root)
                return 0;

            const int thirdSpan = HasFlag(extensions, ChordExtension::Sus4) ? 3
                                  : HasFlag(extensions, ChordExtension::Sus2) ? 1
                                                                              : 2;

            std::size_t toneCount = 0;
            AddScaleTone(scale, rootIdx, 0, 0, degreesPerPeriod, note, noteLen, toneCount);
            AddScaleTone(scale, rootIdx, thirdSpan, 0, degreesPerPeriod, note, noteLen, toneCount);
            AddScaleTone(scale, rootIdx, 4, FifthAlteration(), degreesPerPeriod,
                         note, noteLen, toneCount);

            if (HasFlag(extensions, ChordExtension::Sixth))
                AddScaleTone(scale, rootIdx, 5, 0, degreesPerPeriod, note, noteLen,
                             toneCount);
            if (HasFlag(extensions, ChordExtension::Seventh))
                AddScaleTone(scale, rootIdx, 6, 0, degreesPerPeriod, note, noteLen,
                             toneCount);
            if (HasFlag(extensions, ChordExtension::Ninth))
                AddScaleTone(scale, rootIdx, 8, NinthAlteration(), degreesPerPeriod,
                             note, noteLen, toneCount);
            if (HasFlag(extensions, ChordExtension::Eleventh))
                AddScaleTone(scale, rootIdx, 10, EleventhAlteration(), degreesPerPeriod,
                             note, noteLen, toneCount);
            if (HasFlag(extensions, ChordExtension::Thirteenth))
                AddScaleTone(scale, rootIdx, 12, ThirteenthAlteration(), degreesPerPeriod,
                             note, noteLen, toneCount);

            ApplyInversion(note, toneCount, degreesPerPeriod);

            return toneCount;
        }        

        template <std::size_t N>
        bool GetChordName(const ScaleMap<SCALE_DEGREES> &scale,
                          MString<N> &out,
                          int degreesPerPeriod = 12) const
        {
            const std::size_t rootIdx = scale.GetIndexOfDegree(root);
            if (scale.Count() == 0 || rootIdx >= scale.Count())
                return out.set("?");

            int periodOffset = 0;
            if (scale.GetMappedDegree(rootIdx, periodOffset) != root)
                return out.set("?");

            Note tones[8];
            const std::size_t toneCount =
                GetChordTones(scale, degreesPerPeriod, tones, ArrayLen(tones));
            return BuildChordNameFromTones(scale, rootIdx, root, tones, toneCount,
                                           degreesPerPeriod, extensions, alterations,
                                           out);
        }

        template <std::size_t TEMPERAMENT_DEGREES, std::size_t N>
        bool GetChordName(
            const ScaleMap<SCALE_DEGREES> &scale,
            const Temperament<TEMPERAMENT_DEGREES> &temperament,
            MString<N> &out) const
        {
            const std::size_t rootIdx = scale.GetIndexOfDegree(root);
            if (scale.Count() == 0 || rootIdx >= scale.Count())
                return out.set("?");

            int periodOffset = 0;
            if (scale.GetMappedDegree(rootIdx, periodOffset) != root)
                return out.set("?");

            Note tones[8];
            const std::size_t toneCount = GetChordTones(
                scale, static_cast<int>(temperament.DegreesPerPeriod()), tones,
                ArrayLen(tones));
            if (toneCount < 3)
                return out.set("?");

            MString<8> thirdLabel;
            MString<8> fifthLabel;
            Note qualityFifth = tones[2];
            if (HasFlag(alterations, ChordAlteration::Flat5))
                qualityFifth = static_cast<Note>(qualityFifth + 1);
            else if (HasFlag(alterations, ChordAlteration::Sharp5))
                qualityFifth = static_cast<Note>(qualityFifth - 1);

            if (!temperament.GetIntervalLabel(
                    static_cast<Degree>(wrap(static_cast<int>(tones[1] - root),
                                             static_cast<int>(temperament.DegreesPerPeriod()))),
                    thirdLabel) ||
                !temperament.GetIntervalLabel(
                    static_cast<Degree>(wrap(static_cast<int>(qualityFifth - root),
                                             static_cast<int>(temperament.DegreesPerPeriod()))),
                    fifthLabel))
            {
                return GetChordName(scale, out,
                                    static_cast<int>(temperament.DegreesPerPeriod()));
            }

            ChordQuality quality =
                HasFlag(extensions, ChordExtension::Sus4) ? ChordQuality::Suspended4
                : HasFlag(extensions, ChordExtension::Sus2) ? ChordQuality::Suspended2
                                                            : ClassifyChordQualityFromIntervalLabels(thirdLabel, fifthLabel);
            if (quality == ChordQuality::Unknown)
            {
                const int lowerThird =
                    wrap(static_cast<int>(tones[1] - root),
                         static_cast<int>(temperament.DegreesPerPeriod()));
                const int upperThird =
                    wrap(static_cast<int>(tones[2] - tones[1]),
                         static_cast<int>(temperament.DegreesPerPeriod()));

                int allThirds[SCALE_CHORD_COUNT] = {};
                std::size_t thirdCount = 0;
                for (std::size_t i = 0; i < scale.Count() && i < SCALE_CHORD_COUNT; ++i)
                {
                    int firstPeriod = 0;
                    int thirdPeriod = 0;
                    const int first = scale.GetMappedDegree(i, firstPeriod);
                    const int third = scale.GetMappedDegree(i + 2, thirdPeriod);
                    allThirds[thirdCount++] =
                        (third + thirdPeriod * static_cast<int>(temperament.DegreesPerPeriod())) -
                        (first + firstPeriod * static_cast<int>(temperament.DegreesPerPeriod()));
                }

                quality = ClassifyChordQualityFromThirdStack(
                    lowerThird, upperThird, MinValue(allThirds, thirdCount),
                    MaxValue(allThirds, thirdCount));
            }

            if (!BuildChordNameFromQuality(rootIdx, quality, out))
                return false;
            return AppendExtensionName(out, extensions) &&
                   AppendAlterationName(out, alterations);
        }

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

        template <std::size_t N>
        static bool BuildChordNameFromTones(const ScaleMap<SCALE_DEGREES> &scale,
                                            int scaleIndex,
                                            Note chordRoot,
                                            const Note tones[],
                                            std::size_t toneCount,
                                            int degreesPerPeriod,
                                            ChordExtension extensions,
                                            ChordAlteration alterations,
                                            MString<N> &out)
        {
            Note qualityTones[3] = {};
            const Note *tonesForQuality = tones;
            if (tones && toneCount >= 3)
            {
                for (std::size_t i = 0; i < 3; ++i)
                    qualityTones[i] = tones[i];

                if (HasFlag(alterations, ChordAlteration::Flat5))
                    qualityTones[2] = static_cast<Note>(qualityTones[2] + 1);
                else if (HasFlag(alterations, ChordAlteration::Sharp5))
                    qualityTones[2] = static_cast<Note>(qualityTones[2] - 1);

                tonesForQuality = qualityTones;
            }

            const ChordQuality quality =
                HasFlag(extensions, ChordExtension::Sus4) ? ChordQuality::Suspended4
                : HasFlag(extensions, ChordExtension::Sus2) ? ChordQuality::Suspended2
                                                            : ClassifyChordQuality(scale, scaleIndex, chordRoot, tonesForQuality, toneCount,
                                                                                   degreesPerPeriod);

            switch (quality)
            {
            default:
            {
                if (!BuildChordNameFromQuality(scaleIndex, quality, out))
                    return false;
                return AppendExtensionName(out, extensions) &&
                       AppendAlterationName(out, alterations);
            }
            }
        }

        int FifthAlteration() const
        {
            if (HasFlag(alterations, ChordAlteration::Flat5))
                return -1;
            if (HasFlag(alterations, ChordAlteration::Sharp5))
                return 1;
            return 0;
        }

        int NinthAlteration() const
        {
            if (HasFlag(alterations, ChordAlteration::Flat9))
                return -1;
            if (HasFlag(alterations, ChordAlteration::Sharp9))
                return 1;
            return 0;
        }

        int EleventhAlteration() const
        {
            return HasFlag(alterations, ChordAlteration::Sharp11) ? 1 : 0;
        }

        int ThirteenthAlteration() const
        {
            return HasFlag(alterations, ChordAlteration::Flat13) ? -1 : 0;
        }

        void ApplyInversion(Note note[], std::size_t toneCount, int degreesPerPeriod) const
        {
            if (!note || toneCount < 2 || inversion == 0)
                return;

            if (inversion > 0)
            {
                const std::size_t turns =
                    static_cast<std::size_t>(inversion) % toneCount;
                for (std::size_t i = 0; i < turns; ++i)
                {
                    const Note moved =
                        static_cast<Note>(note[0] + degreesPerPeriod);
                    for (std::size_t j = 1; j < toneCount; ++j)
                        note[j - 1] = note[j];
                    note[toneCount - 1] = moved;
                }
                return;
            }

            const std::size_t turns =
                static_cast<std::size_t>(-inversion) % toneCount;
            for (std::size_t i = 0; i < turns; ++i)
            {
                const Note moved =
                    static_cast<Note>(note[toneCount - 1] - degreesPerPeriod);
                for (std::size_t j = toneCount - 1; j > 0; --j)
                    note[j] = note[j - 1];
                note[0] = moved;
            }
        }

        static void AddScaleTone(const ScaleMap<SCALE_DEGREES> &scale,
                                 std::size_t rootIdx,
                                 int scaleSpan,
                                 int alteration,
                                 int degreesPerPeriod,
                                 Note note[],
                                 std::size_t noteLen,
                                 std::size_t &toneCount)
        {
            if (toneCount >= noteLen)
                return;

            int periodOffset = 0;
            const Note mapped =
                static_cast<Note>(scale.GetMappedDegree(rootIdx + scaleSpan,
                                                        periodOffset));
            note[toneCount++] = static_cast<Note>(
                mapped + (periodOffset * degreesPerPeriod) + alteration);
        }

        static ChordQuality ClassifyChordQuality(const ScaleMap<SCALE_DEGREES> &scale,
                                                 int scaleIndex,
                                                 Note chordRoot,
                                                 const Note tones[],
                                                 std::size_t toneCount,
                                                 int degreesPerPeriod)
        {
            if (!tones || toneCount < 3 || degreesPerPeriod <= 0 || scale.Count() < 3)
                return ChordQuality::Unknown;

            int thirdIntervals[SCALE_CHORD_COUNT] = {};
            int fifthIntervals[SCALE_CHORD_COUNT] = {};
            std::size_t thirdCount = 0;
            std::size_t fifthCount = 0;

            for (std::size_t i = 0; i < scale.Count() && i < SCALE_CHORD_COUNT; ++i)
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
            for (std::size_t i = 0; i < fifthCount; ++i)
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

        template <std::size_t N>
        static bool BuildChordNameFromQuality(int scaleIndex,
                                              ChordQuality quality,
                                              MString<N> &out)
        {
            switch (quality)
            {
            case ChordQuality::Major:
                return out.set(RomanNumeralForScaleIndex(scaleIndex));
            case ChordQuality::Minor:
                return out.set(LowercaseRomanNumeralForScaleIndex(scaleIndex));
            case ChordQuality::Diminished:
                return out.set(LowercaseRomanNumeralForScaleIndex(scaleIndex)) &&
                       out.append("_o");
            case ChordQuality::Augmented:
                return out.set(RomanNumeralForScaleIndex(scaleIndex)) &&
                       out.append("+");
            case ChordQuality::Suspended2:
                return out.set(RomanNumeralForScaleIndex(scaleIndex)) &&
                       out.append("sus2");
            case ChordQuality::Suspended4:
                return out.set(RomanNumeralForScaleIndex(scaleIndex)) &&
                       out.append("sus4");
            case ChordQuality::PowerChord:
                return out.set(RomanNumeralForScaleIndex(scaleIndex)) &&
                       out.append("5");
            case ChordQuality::Unknown:
            default:
                return out.set(RomanNumeralForScaleIndex(scaleIndex)) &&
                       out.append("?");
            }
        }

        template <std::size_t N>
        static bool AppendExtensionName(MString<N> &name, ChordExtension extensions)
        {
            if (HasFlag(extensions, ChordExtension::Thirteenth))
                return name.append("13");
            else if (HasFlag(extensions, ChordExtension::Eleventh))
                return name.append("11");
            else if (HasFlag(extensions, ChordExtension::Ninth))
                return name.append("9");
            else if (HasFlag(extensions, ChordExtension::Seventh))
                return name.append("7");
            else if (HasFlag(extensions, ChordExtension::Sixth))
                return name.append("6");
            return true;
        }

        template <std::size_t N>
        static bool AppendAlterationName(MString<N> &name, ChordAlteration alterations)
        {
            if (HasFlag(alterations, ChordAlteration::Flat5))
                if (!name.append("b5"))
                    return false;
            if (HasFlag(alterations, ChordAlteration::Sharp5))
                if (!name.append("#5"))
                    return false;
            if (HasFlag(alterations, ChordAlteration::Flat9))
                if (!name.append("b9"))
                    return false;
            if (HasFlag(alterations, ChordAlteration::Sharp9))
                if (!name.append("#9"))
                    return false;
            if (HasFlag(alterations, ChordAlteration::Sharp11))
                if (!name.append("#11"))
                    return false;
            if (HasFlag(alterations, ChordAlteration::Flat13))
                if (!name.append("b13"))
                    return false;
            return true;
        }

        static int GetScaleSpanInterval(const ScaleMap<SCALE_DEGREES> &scale,
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

        static int MinValue(const int values[], std::size_t count)
        {
            if (!values || count == 0)
                return 0;

            int best = values[0];
            for (std::size_t i = 1; i < count; ++i)
            {
                if (values[i] < best)
                    best = values[i];
            }
            return best;
        }

        static int MaxValue(const int values[], std::size_t count)
        {
            if (!values || count == 0)
                return 0;

            int best = values[0];
            for (std::size_t i = 1; i < count; ++i)
            {
                if (values[i] > best)
                    best = values[i];
            }
            return best;
        }

        static int GreatestValueLessThan(const int values[], std::size_t count, int limit)
        {
            int best = 0;
            bool found = false;
            for (std::size_t i = 0; i < count; ++i)
            {
                if (values[i] < limit && (!found || values[i] > best))
                {
                    best = values[i];
                    found = true;
                }
            }
            return found ? best : limit;
        }

        static int LeastValueGreaterThan(const int values[], std::size_t count, int limit)
        {
            int best = 0;
            bool found = false;
            for (std::size_t i = 0; i < count; ++i)
            {
                if (values[i] > limit && (!found || values[i] < best))
                {
                    best = values[i];
                    found = true;
                }
            }
            return found ? best : limit;
        }

        static const char *RomanNumeralForScaleIndex(int scaleIndex)
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

        static const char *LowercaseRomanNumeralForScaleIndex(int scaleIndex)
        {
            switch (wrap(scaleIndex, SCALE_CHORD_COUNT))
            {
            case 0:
                return "i";
            case 1:
                return "ii";
            case 2:
                return "iii";
            case 3:
                return "iv";
            case 4:
                return "v";
            case 5:
                return "vi";
            case 6:
                return "vii";
            default:
                return "?";
            }
        }

        static bool MatchesIntervalLabel(const char *actual, const char *expected)
        {
            if (!actual || !expected)
                return false;

            std::size_t i = 0;
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
        for (std::size_t i = 0; i < SCALE_CHORD_COUNT; ++i)
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
