/* SPDX-License-Identifier: CC0-1.0 */
#pragma once

#ifdef USE_DEBUG
#include <TTY.h>
#include <iomanip>
#include <iostream>
#endif

#include <Music/Music.h>

namespace Music
{
  template <std::size_t MAX_DEGREES, std::size_t SCALE_DEGREES,
            std::size_t MAX_EVENTS>
  std::size_t GenerateWeightedChordEvents(
      const Setup<MAX_DEGREES, SCALE_DEGREES> &setup, Note tonic,
      NoteValue granularity,
      ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS> &chords)
  {

    if (chords.Capacity() == 0 || setup.bars <= 0 ||
        granularity == NoteValue::None)
    {
      return 0;
    }

    const std::size_t totalEvents =
        (static_cast<std::size_t>(setup.bars) *
         static_cast<std::size_t>(setup.timeSignature.beats) *
         static_cast<std::size_t>(setup.timeSignature.beatValue)) /
        static_cast<std::size_t>(granularity);
    const std::size_t chordsToEmit = min(totalEvents, chords.Capacity());
    ScaleDegree degree =
        GetWeightedStartingChord(setup.scaleMap.GetHarmonicMode());
    const bool hasScale = (setup.scaleMap.Count() >= SCALE_CHORD_COUNT);

    chords.Clear();
    for (std::size_t i = 0; i < chordsToEmit; ++i)
    {
      const int degreeIndex =
          ScaleDegreeIndex(degree, setup.scaleMap.GetHarmonicMode());
      Note rootOffset = ScaleDegreeToSemitone(degree);
      if (hasScale && degreeIndex >= 0)
      {
        int scalePeriodOffset = 0;
        rootOffset = static_cast<Note>(
            setup.scaleMap.GetMappedDegree(degreeIndex, scalePeriodOffset));
      }

      ChordExtension extensions = (degree == ScaleDegree::V)
                                      ? ChordExtension::Seventh
                                      : Music::ChordExtension{};
      chords.Emplace(tonic + rootOffset, granularity, extensions);

      degree = GetWeightedNextChord(degree, setup.scaleMap.GetHarmonicMode());
    }
    return chords.Count();
  }

  template <std::size_t MAX_DEGREES, std::size_t SCALE_DEGREES,
            std::size_t MAX_EVENTS>
  std::size_t GenerateStandardChordEvents(
      const Setup<MAX_DEGREES, SCALE_DEGREES> &setup, NoteValue granularity,
      ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS> &chords)
  {
    return GenerateWeightedChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(
        setup, 0, granularity, chords);
  }

  template <std::size_t MAX_DEGREES, std::size_t SCALE_DEGREES,
            std::size_t MAX_EVENTS>
  std::size_t GenerateEventsFromPattern(
      const PatternEventSet<MAX_EVENTS> &pattern,
      const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS> &chords,
      const TimeSignature &ts, const Temperament<MAX_DEGREES> &temperament,
      const ScaleMap<SCALE_DEGREES> &scale, int bars, NoteValue granularity,
      NoteEventSet<MAX_EVENTS> &events)
  {
    (void)bars;
    events.Clear();
    if (chords.Count() == 0)
    {
      return 0;
    }

    int pulses = 0;
    int chordPulses = 0;
    int ppb = ts.GetPulsesPerBar();
    std::size_t chordIdx = 0;

    Note tones[20];
    std::size_t toneCount = chords[chordIdx].GetChordTones(
        scale, static_cast<int>(temperament.DegreesPerPeriod()), tones,
        ArrayLen(tones));

    for (std::size_t i = 0; i < pattern.Count() && !events.AtCapacity(); i++,
                     pulses = pulses + granularity,
                     chordPulses = chordPulses + granularity)
    {
      if (chordPulses >= static_cast<int>(chords[chordIdx].value) &&
          chordIdx < chords.Count() - 1)
      {
        chordIdx++;
        chordPulses = 0;

        toneCount = chords[chordIdx].GetChordTones(
            scale, static_cast<int>(temperament.DegreesPerPeriod()), tones,
            ArrayLen(tones));
      }

      if (pattern[i])
      {
        if (pulses % ppb == 0)
        {
          events.Emplace(tones[0], 0, granularity);
        }
        else
        {
          int idx = randomRange(0, static_cast<int>(toneCount) - 1);
          events.Emplace(tones[idx], 0, granularity);
        }
      }
      else
      {
        if (pulses % ppb != 0 && events.Count() > 1 &&
            events[events.Count() - 1].note == REST)
        {
          events[events.Count() - 1].value += granularity;
        }
        else
        {
          events.Emplace(REST, 0, granularity);
        }
      }
    }
    return events.Count();
  }

  template <std::size_t MAX_DEGREES, std::size_t MAX_EVENTS>
  NoteEventScore ScoreNoteEvents(const Temperament<MAX_DEGREES> &t,
                                 const NoteEventSet<MAX_EVENTS> &events)
  {
    NoteEventScore score;
    if (events.Count() == 0)
    {
      return score;
    }

    int totalPulses = 0;
    int pitchedPulses = 0;
    int restPulses = 0;
    int pitchedEventCount = 0;
    int shortPitchedEvents = 0;
    int longPitchedEvents = 0;
    int repeatedNotes = 0;
    int repeatedDurations = 0;
    int phraseCount = 0;
    int consonantPhraseStarts = 0;
    int consonantPhraseEnds = 0;
    int stepMoves = 0;
    int hugeLeaps = 0;
    int transitionCount = 0;

    const int longThreshold = static_cast<int>(NoteValue::Half);
    const int shortThreshold = static_cast<int>(NoteValue::Eighth);

    Note previousPitchedNote = REST;
    NoteValue previousPitchedDuration = NoteValue::None;
    bool inPhrase = false;

    for (std::size_t i = 0; i < events.Count(); i++)
    {
      int eventPulses = static_cast<int>(events[i].value);
      totalPulses += eventPulses;

      if (!events[i].IsPitched())
      {
        restPulses += eventPulses;
        inPhrase = false;
        continue;
      }

      pitchedPulses += eventPulses;
      pitchedEventCount++;

      if (!inPhrase)
      {
        phraseCount++;
        if (t.IsConsonant(t.GetNoteDistance(0, events[i].note)))
        {
          consonantPhraseStarts++;
        }
        inPhrase = true;
      }

      if (eventPulses <= shortThreshold)
      {
        shortPitchedEvents++;
      }
      if (eventPulses >= longThreshold)
      {
        longPitchedEvents++;
      }

      if (previousPitchedNote != REST)
      {
        const int interval =
            t.GetNoteDistance(previousPitchedNote, events[i].note);
        if (events[i].note == previousPitchedNote)
        {
          repeatedNotes++;
        }
        else if (interval <= 2)
        {
          stepMoves++;
        }
        else if (interval >= 7)
        {
          if (interval >= 10)
          {
            hugeLeaps++;
          }
        }

        if (events[i].value == previousPitchedDuration)
        {
          repeatedDurations++;
        }

        transitionCount++;
      }

      const bool nextStartsNewPhrase = (i + 1 >= events.Count()) ||
                                       !events[i + 1].IsPitched() ||
                                       eventPulses >= longThreshold;

      if (nextStartsNewPhrase &&
          t.IsConsonant(t.GetNoteDistance(0, events[i].note)))
      {
        consonantPhraseEnds++;
      }

      previousPitchedNote = events[i].note;
      previousPitchedDuration = events[i].value;
    }

    if (pitchedPulses == 0 || totalPulses == 0)
    {
      return score;
    }

    const int densityPct =
        static_cast<int>((pitchedPulses * 100UL) / totalPulses);
    const int restPct = static_cast<int>((restPulses * 100UL) / totalPulses);
    const int phraseStartPct =
        phraseCount > 0
            ? static_cast<int>((consonantPhraseStarts * 100UL) / phraseCount)
            : 0;
    const int phraseEndPct =
        phraseCount > 0
            ? static_cast<int>((consonantPhraseEnds * 100UL) / phraseCount)
            : 0;
    const int repeatPct =
        transitionCount > 0
            ? static_cast<int>(((repeatedNotes + repeatedDurations) * 100UL) /
                               (transitionCount * 2UL))
            : 0;
    const int stepPct =
        transitionCount > 0
            ? static_cast<int>((stepMoves * 100UL) / transitionCount)
            : 0;
    const int hugeLeapPct =
        transitionCount > 0
            ? static_cast<int>((hugeLeaps * 100UL) / transitionCount)
            : 0;
    const int phraseDensity =
        pitchedEventCount > 0
            ? static_cast<int>((phraseCount * 100UL) / pitchedEventCount)
            : 0;
    const int rhythmicContrast =
        (shortPitchedEvents + longPitchedEvents) > 0
            ? static_cast<int>(
                  (min(shortPitchedEvents, longPitchedEvents) * 200UL) /
                  (shortPitchedEvents + longPitchedEvents))
            : 0;

    score.density = clamp(100 - (abs(densityPct - 55) * 2), 0, 100);
    score.rests = clamp(100 - (abs(restPct - 25) * 2), 0, 100);
    score.cadence = clamp((phraseStartPct + (phraseEndPct * 2)) / 3, 0, 100);
    score.repetition = clamp(100 - (abs(repeatPct - 30) * 2), 0, 100);
    score.contour = clamp(100 - abs(stepPct - 65) - (hugeLeapPct * 2), 0, 100);
    score.phrase = clamp(100 - (abs(phraseDensity - 35) * 2), 0, 100);
    score.rhythm = clamp(rhythmicContrast, 0, 100);

    score.overall =
        ((score.density * 20U) + (score.rests * 10U) + (score.cadence * 25U) +
         (score.repetition * 15U) + (score.contour * 15U) + (score.phrase * 10U) +
         (score.rhythm * 5U)) /
        100U;

    return score;
  }

#ifdef USE_DEBUG
  template <std::size_t MAX_EVENTS>
  void DebugPattern(const TimeSignature &ts, NoteValue granularity,
                    const PatternEventSet<MAX_EVENTS> &pattern)
  {
    std::cout << TTY_FG_YELLOW << "PATTERN EVENTS" << TTY_RESET << "\t"
              << ts.beats << "/" << ts.GetDenominator()
              << "\tDensity: " << pattern.GetDensity() << std::endl;
    if (pattern.Count() == 0 || granularity == NoteValue::None)
    {
      return;
    }

    const int g = static_cast<int>(granularity);
    const int kPerBar = (ts.beats * static_cast<int>(ts.beatValue)) / g;
    if (kPerBar > 0)
    {
      for (std::size_t i = 0; i < pattern.Count(); i++)
      {
        if (i % kPerBar == 0)
        {
          std::cout << "|";
        }
        std::cout << (pattern[i] ? "*" : "-");
      }
    }
    std::cout << "|" << '\n';
  }

  template <std::size_t MAX_DEGREES, std::size_t MAX_EVENTS>
  void DebugNoteEvents(const Temperament<MAX_DEGREES> &t, const TimeSignature &ts,
                       const NoteEventSet<MAX_EVENTS> &events)
  {
    std::cout << TTY_FG_CYAN << "NOTE EVENTS" << TTY_RESET
              << " Density: " << events.GetDensity() << std::endl;

    int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int v = 0;
    char noteLabel[8];
    for (std::size_t i = 0; i < events.Count(); i++)
    {
      if (!events[i].IsPitched())
      {
        std::cout << TTY_FAINT;
      }

      t.GetNoteLabel(events[i].note, noteLabel, sizeof(noteLabel));
      std::cout << std::right << std::setw(6) << noteLabel
                << static_cast<int>(events[i].period) << "-" << std::left
                << std::setw(6) << GetNoteValueText(events[i].value);

      if (!events[i].IsPitched())
      {
        std::cout << TTY_RESET;
      }

      v = events[i].value + v;
      if (v % vPerBar == 0)
      {
        std::cout << '\n';
      }
    }
  }

  template <std::size_t MAX_DEGREES, std::size_t SCALE_DEGREES,
            std::size_t MAX_EVENTS>
  void DebugChordEvents(
      const Setup<MAX_DEGREES, SCALE_DEGREES> &setup,
      const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS> &chords)
  {
    int vPerBar = setup.timeSignature.beats *
                  static_cast<int>(setup.timeSignature.beatValue);
    int v = 0;
    char noteLabel[8];
    MString<16> chordName;
    std::cout << TTY_FG_GREEN << "CHORD EVENTS" << TTY_RESET
              << " Density: " << chords.GetDensity() << std::endl;
    for (std::size_t i = 0; i < chords.Count(); i++)
    {
      Note tones[8];
      std::size_t toneCount = chords[i].GetChordTones(
          setup.scaleMap, static_cast<int>(setup.temperament.DegreesPerPeriod()),
          tones, ArrayLen(tones));

      if (v % vPerBar == 0)
      {
        std::cout << "| ";
      }
      chords[i].GetChordName(setup.scaleMap, setup.temperament, chordName);
      std::cout << chordName << " (";

      for (std::size_t j = 0; j < toneCount; j++)
      {
        setup.temperament.GetNoteLabel(tones[j], noteLabel, sizeof(noteLabel));
        std::cout << noteLabel;
        if (j < toneCount - 1)
        {
          std::cout << "|";
        }
      }

      std::cout << ")-" << GetNoteValueText(chords[i].value) << " ";
      v = chords[i].value + v;
    }
    std::cout << "|" << '\n';
  }
#endif

} // namespace Music
