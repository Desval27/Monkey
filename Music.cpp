/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Music.cpp
 * @brief General Music Functions
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @brief
 * 
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#include <Monkey.h>
#include <Music/Music.h>

namespace Music
{
// C  C#  D  D#  E   F  F#  G  G#   A   A#   B
// I  ii  II iii III IV TT  V  V+   vi  vii  VII
// 0  1   2  3   4   5  6   7  8    9   10   11

static int weightedIndexFromRow(const float *weights, size_t count)
{
    if(!weights || count == 0)
        return 0;

    float sum = 0.0f;
    for(size_t i = 0; i < count; ++i)
    {
        if(weights[i] > 0.0f)
            sum += weights[i];
    }

    if(sum <= 0.0f)
        return 0;

    const float unitRandom = randomRange(0.0f, 0.999999f);
    const float pick       = unitRandom * sum;
    float       accum      = 0.0f;

    for(size_t i = 0; i < count; ++i)
    {
        const float w = (weights[i] > 0.0f) ? weights[i] : 0.0f;
        accum += w;
        if(pick < accum)
            return static_cast<int>(i);
    }

    return static_cast<int>(count - 1);
}

ScaleDegree getWeightedStartingChord(HarmonicMode mode)
{
    const int index = weightedIndexFromRow(startingChordWeightsForMode(mode),
                                           SCALE_CHORD_COUNT);
    return scaleDegreeFromIndex(index, mode);
}

ScaleDegree getWeightedNextChord(ScaleDegree fromDegree, HarmonicMode mode)
{
    const int fromIndex = scaleDegreeIndex(fromDegree, mode);
    if(fromIndex < 0)
        return getWeightedStartingChord(mode);

    const ChordProbability *table = chordProbabilityTableForMode(mode);
    const int               toIdx
        = weightedIndexFromRow(table[fromIndex].toDegree, SCALE_CHORD_COUNT);
    return scaleDegreeFromIndex(toIdx, mode);
}

size_t generatePhrygianDominantChordEvents(ChordEvent *eventsOut,
                                           size_t      eventSize)
{
    // Key chords (I, bII, #iii(dim), iv, v(dim), +V, vii)
    // Common Progressions:
    //      I(7) - bII
    //      I(7) - bII - vi - I(7)
    // Andalusian Cadance:
    //      vi - III - bII - I(7)
    // Jazz/Tension:
    //      I(7b9) - vii(7)
    size_t eventIdx = 0;

    if(eventSize < 10)
        return eventIdx;

    // Bar 1
    eventsOut[eventIdx].root  = 0;
    eventsOut[eventIdx].value = NoteValue::Whole;
    eventIdx++;

    // Bar 2
    eventsOut[eventIdx].root  = 1;
    eventsOut[eventIdx].value = NoteValue::Whole;
    eventIdx++;

    // Bar 3
    eventsOut[eventIdx].root  = 0;
    eventsOut[eventIdx].value = NoteValue::Whole;
    eventIdx++;

    // Bar 4
    eventsOut[eventIdx].root  = 1;
    eventsOut[eventIdx].value = NoteValue::Whole;
    eventIdx++;

    // Bar 5
    eventsOut[eventIdx].root  = 0;
    eventsOut[eventIdx].value = NoteValue::Whole;
    eventIdx++;

    // Bar 6
    eventsOut[eventIdx].root  = 1;
    eventsOut[eventIdx].value = NoteValue::Whole;
    eventIdx++;

    // Bar 7
    eventsOut[eventIdx].root  = 5;
    eventsOut[eventIdx].value = NoteValue::Half;
    eventIdx++;

    eventsOut[eventIdx].root  = 3;
    eventsOut[eventIdx].value = NoteValue::Half;
    eventIdx++;

    // Bar 8
    eventsOut[eventIdx].root  = 1;
    eventsOut[eventIdx].value = NoteValue::Half;
    eventIdx++;

    eventsOut[eventIdx].root  = 0;
    eventsOut[eventIdx].value = NoteValue::Half;
    eventIdx++;

    return eventIdx;
}

size_t generateLydian1(const TimeSignature &ts,
                       int                  bars,
                       ChordEvent          *eventsOut,
                       size_t               eventSize)
{
    // I - II - I - V
    size_t     eventIdx = 0;
    const Note notes[]  = {0, 2, 0, 7};

    for(eventIdx = 0;
        eventIdx < static_cast<size_t>(bars) && eventIdx < eventSize;
        eventIdx++)
    {
        eventsOut[eventIdx].root  = notes[eventIdx % ArrayLen(notes)];
        eventsOut[eventIdx].value = NoteValue::Whole;
    }

    return eventIdx;
}


size_t generateStandardChordEvents(const TimeSignature &ts,
                                   const ScaleMap      &scale,
                                   int                  bars,
                                   HarmonicMode         mode,
                                   NoteValue            granularity,
                                   ChordEvent          *eventsOut,
                                   size_t               eventsMax)
{
    return generateWeightedChordEvents(
        ts, scale, bars, mode, 0, granularity, eventsOut, eventsMax);
}

size_t generateWeightedChordEvents(const TimeSignature &ts,
                                   const ScaleMap      &scale,
                                   int                  bars,
                                   HarmonicMode         mode,
                                   Note                 tonic,
                                   NoteValue            granularity,
                                   ChordEvent          *eventsOut,
                                   size_t               eventsMax)
{
    (void)ts;
    if(!eventsOut || eventsMax == 0 || bars <= 0
       || granularity == NoteValue::None)
        return 0;

    const size_t barsToEmit = min(static_cast<size_t>(bars), eventsMax);
    ScaleDegree  degree     = getWeightedStartingChord(mode);
    const bool   hasScale   = (scale.count() >= SCALE_CHORD_COUNT);

    for(size_t i = 0; i < barsToEmit; ++i)
    {
        const int degreeIndex = scaleDegreeIndex(degree, mode);
        Note      rootOffset  = scaleDegreeToSemitone(degree);
        if(hasScale && degreeIndex >= 0)
        {
            int scalePeriodOffset = 0;
            rootOffset            = static_cast<Note>(
                scale.mappedDegree(degreeIndex, scalePeriodOffset));
        }

        eventsOut[i].root      = tonic + rootOffset;
        eventsOut[i].value     = granularity;
        eventsOut[i].inversion = 0;

        degree = getWeightedNextChord(degree, mode);
    }

    return barsToEmit;
}

size_t generateChordEventsFromPattern(const bool *pattern,
                                      size_t      patternSize,
                                      NoteValue   granularity,
                                      ChordEvent *eventsOut,
                                      size_t      eventsMax)
{
    if(pattern == nullptr || patternSize == 0 || granularity == NoteValue::None)
        return 0;

    size_t maxSize = min(patternSize, eventsMax);

    size_t eventCount = 0;
    size_t idx        = 0;
    eventsOut[eventCount].root
        = REST; // By default; normal would be first event is a hit but this allows for a "gap" event if not.
    eventsOut[eventCount].value = NoteValue::None;
    while(idx < maxSize)
    {
        if(pattern[idx])
        {
            if(idx > 0)
                eventCount++;

            switch(eventCount % 8)
            {
                case 0: eventsOut[eventCount].root = 0; break;
                case 1: eventsOut[eventCount].root = 5; break;
                case 2: eventsOut[eventCount].root = 7; break;
                case 3: eventsOut[eventCount].root = 5; break;
                case 4: eventsOut[eventCount].root = 0; break;
                case 5: eventsOut[eventCount].root = 7; break;
                case 6: eventsOut[eventCount].root = 5; break;
                case 7: eventsOut[eventCount].root = 7; break;
                default: eventsOut[eventCount].root = REST; break;
            }
            eventsOut[eventCount].value = granularity;
        }
        else
        {
            eventsOut[eventCount].value
                = eventsOut[eventCount].value + granularity;
        }
        idx++;
    }
    return eventCount + 1;
}

size_t generatePattern(const TimeSignature &ts,
                       int                  bars,
                       float                density,
                       NoteValue            granularity,
                       bool                *out,
                       size_t               outMax)
{
    DEBUG_GOT_HERE();

    if(out == nullptr || outMax == 0 || bars == 0
       || ts.beatValue == NoteValue::None || granularity == NoteValue::None)
        return 0; // Sanity check

    int n = (ts.beats * bars)
            * (static_cast<int>(ts.beatValue) / static_cast<int>(granularity));
    int k = n * density;
    DPRINTF("Generating Events %d in %d Slots using Density %f\n", k, n, density);
    return buildEuclid(k, n, 1, out, outMax);
}

#if USE_DEBUG
//#if true
void debugPattern(const TimeSignature &ts,
                  NoteValue            granularity,
                  const bool          *pattern,
                  size_t               patternSize)
{
    if(patternSize <= 0 || granularity == NoteValue::None)
        return;

    int kPerBar
        = ts.beats
          * (static_cast<int>(ts.beatValue) / static_cast<int>(granularity));
    DPRINTF("%d events per bar\n", kPerBar);
    for(size_t i = 0; i < patternSize; i++)
    {
        if(i % kPerBar == 0)
            DPRINTF("|");
        if(pattern[i])
            DPRINTF("*");
        else
            DPRINTF("-");
    }
    DPRINTF("|\n");
}

void debugChordEvents(const Temperament   &t,
                      const TimeSignature &ts,
                      const ChordEvent    *events,
                      size_t               eventsSize)
{
    int  vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int  v       = 0;
    char noteLabel[8];
    for(size_t i = 0; i < eventsSize; i++)
    {
        t.getNoteLabel(events[i].root, noteLabel, sizeof(noteLabel));
        if(v % vPerBar == 0)
            DPRINTF("| ");
        DPRINTF("%s(%d)(%d) ",
              noteLabel,
              events[i].root,
              static_cast<int>(events[i].value));

        v = events[i].value + v;
    }
    DPRINTF("|\n");
}

void debugNoteEvents(const TimeSignature &ts,
                     const NoteEvent     *events,
                     size_t               eventsSize)
{
    int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int v       = 0;
    for(size_t i = 0; i < eventsSize; i++)
    {
        if(v % vPerBar == 0)
            DPRINTF("| ");
        DPRINTF("%d(%d) ", events[i].note, static_cast<int>(events[i].value));

        v = events[i].value + v;
    }
    DPRINTF("|\n");
}

#endif
} // namespace Music
