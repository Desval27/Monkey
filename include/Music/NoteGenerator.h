/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Music.h
 * @brief General Music Definitions.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/MusicConfig.h>
#include <Music/MusicHelpers.h>
#include <Music/NoteValue.h>
#include <Music/TimeSignature.h>
#include <Music/Temperament.h>
#include <Music/ScaleMap.h>
#include <Music/EventSet.h>
#include <Music/Tables.h>

namespace Music
{
    template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
    class NullNoteGenerator
    {
    public:
        static size_t GeneratePattern(const PatternEventSet<MAX_EVENTS> &pattern,
                                      const ChordEventSet<MAX_EVENTS> &chords,
                                      const TimeSignature &ts,
                                      const Temperament &temperament,
                                      const ScaleMap &scale,
                                      int bars,
                                      NoteValue granularity,
                                      NoteEventSet<MAX_EVENTS> &events)
        {
            events.Clear();
            return events.Count();
        }
    };

    template <size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
    class StyleANoteGenerator
    {
    public:
        static size_t GenerateEvents(const PatternEventSet<MAX_EVENTS> &pattern,
                                          const ChordEventSet<MAX_EVENTS> &chords,
                                          const TimeSignature &ts,
                                          const Temperament &temperament,
                                          const ScaleMap &scale,
                                          int bars,
                                          NoteValue granularity,
                                          NoteEventSet<MAX_EVENTS> &events)
        {
            events.Clear();
            if (chords.Count() == 0)
                return 0;

            int pulses = 0;
            int chordPulses = 0;
            int ppb = ts.GetPulsesPerBar();
            size_t chordIdx = 0;

            Note tones[20];
            size_t toneCount = chords[chordIdx].GetChordTones(
                scale, static_cast<int>(temperament.DegreesPerPeriod()), tones,
                ArrayLen(tones));

            for (size_t i = 0; i < pattern.Count() && !events.AtCapacity(); i++, pulses = pulses + granularity, chordPulses = chordPulses + granularity)
            {
                if (chordPulses >= static_cast<int>(chords[chordIdx].value) && chordIdx < chords.Count() - 1)
                {
                    chordIdx++;
                    chordPulses = 0;

                    toneCount = chords[chordIdx].GetChordTones(
                        scale, static_cast<int>(temperament.DegreesPerPeriod()), tones,
                        ArrayLen(tones));
                }

                if (pattern[i])
                {
                    // Is this at the start of a bar?  Then start with a chord
                    if (pulses % ppb == 0)
                    {
                        // Start with a random chord tone.
                        int idx = randomRange(0, static_cast<int>(toneCount) - 1);
                        events.Emplace(tones[0], 0, granularity);
                    }
                    else
                    {
                        int periodOffset = 0;
                        float unitRandom = randomRange(0.0f, 0.999999f);
                        //
                        // Our weights need to be injected at some point.
                        //
                        Note n = scale.GetWeightedNote(unitRandom, periodOffset, SCALE_WEIGHTS_7_CHORD_TONE_HEAVY, ArrayLen(SCALE_WEIGHTS_7_CHORD_TONE_HEAVY));

                        // If our last event was the same note then (for now) just add to the original duraton
                        // Or just a random occurance
                        // Unless the new event is at the start of a bar
                        // And also unless the new note value leads to something odd (dotted dotted, etc.)
                        if ((pulses % ppb != 0) && (events.Count() > 1) && ((events[events.Count() - 1].note == n) || (randomRange(0.0f, 1.0f) < 0.4f)) && !IsNoteValueWeird(static_cast<NoteValue>(events[events.Count() - 1].value + granularity)))
                        {
                            events[events.Count() - 1].value += granularity;
                        }
                        else
                        {
                            events.Emplace(n, periodOffset, granularity);
                        }
                    }
                }
                else
                {
                    // If our last event was also a rest then just add to its value
                    // Unless the new event is at the start of a bar
                    if ((pulses % ppb != 0 && events.Count() > 1) && (events[events.Count() - 1].note == REST) && !IsNoteValueWeird(static_cast<NoteValue>(events[events.Count() - 1].value + granularity)))
                        events[events.Count() - 1].value += granularity;
                    else
                        events.Emplace(REST, 0, granularity);
                }
            }

            // Do we have any remainders?  If so then create a single rest
            // of that value if we have any additional event capacity.
            int t = bars * ts.GetPulsesPerBar();
            int r = t - events.GetTotalEventPulses();
            if (r > 0)
            {
                // Do we need to add a new event or just tack on to the last one
                // if it is already a rest?
                if (events.Count() > 0 && events[events.Count() - 1].note == REST)
                    events[events.Count() - 1].value += r;
                else if (!events.AtCapacity())
                    events.Emplace(REST, 0, static_cast<NoteValue>(r));
                // Otherwise we're just goint to lose it for now.
            }
            return events.Count();
        }
    };

} // namespace Music
