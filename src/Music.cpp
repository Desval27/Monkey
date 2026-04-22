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
#include <Music/Music.h>

namespace Music
{
  // C  C#  D  D#  E   F  F#  G  G#   A   A#   B
  // I  ii  II iii III IV TT  V  V+   vi  vii  VII
  // 0  1   2  3   4   5  6   7  8    9   10   11

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param weights
  /// @param count
  /// @return
  static int WeightedIndexFromRow(const float *weights, size_t count)
  {
    if (!weights || count == 0)
      return 0;

    float sum = 0.0f;
    for (size_t i = 0; i < count; ++i)
    {
      if (weights[i] > 0.0f)
        sum += weights[i];
    }

    if (sum <= 0.0f)
      return 0;

    const float unitRandom = randomRange(0.0f, 0.999999f);
    const float pick = unitRandom * sum;
    float accum = 0.0f;

    for (size_t i = 0; i < count; ++i)
    {
      const float w = (weights[i] > 0.0f) ? weights[i] : 0.0f;
      accum += w;
      if (pick < accum)
        return static_cast<int>(i);
    }

    return static_cast<int>(count - 1);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param mode
  /// @return
  ScaleDegree GetWeightedStartingChord(HarmonicMode mode)
  {
    const int index = WeightedIndexFromRow(StartingChordWeightsForMode(mode),
                                           SCALE_CHORD_COUNT);
    return ScaleDegreeFromIndex(index, mode);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param fromDegree
  /// @param mode
  /// @return
  ScaleDegree GetWeightedNextChord(ScaleDegree fromDegree, HarmonicMode mode)
  {
    const int fromIndex = ScaleDegreeIndex(fromDegree, mode);
    if (fromIndex < 0)
      return GetWeightedStartingChord(mode);

    const ChordProbability *table = ChordProbabilityTableForMode(mode);
    const int toIdx =
        WeightedIndexFromRow(table[fromIndex].toDegree, SCALE_CHORD_COUNT);
    return ScaleDegreeFromIndex(toIdx, mode);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param ts
  /// @param bars
  /// @param density
  /// @param granularity
  /// @param pattern
  /// @return
  size_t GeneratePattern(const TimeSignature &ts, int bars, float density,
                         NoteValue granularity, PatternEventSet<> &pattern)
  {
    if (pattern.Capacity() == 0 || bars == 0 || ts.beatValue == NoteValue::None || granularity == NoteValue::None)
      return 0; // Sanity check

    int n = (ts.beats * bars) * (static_cast<int>(ts.beatValue) / static_cast<int>(granularity));
	int maxLen = min(n, static_cast<int>(pattern.Capacity()));
	for (int i = 0; i < maxLen; i++)
	{
	  pattern.Emplace(randomRange(0.0f, 1.0f) < density);
	}
	return pattern.Count();

    
    //int n = (ts.beats * bars) *
            //(static_cast<int>(ts.beatValue) / static_cast<int>(granularity));
    //int k = n * density;
    //return BuildEuclid(k, n, 1, pattern);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param events
  /// @return
  NoteEventScore ScoreNoteEvents(const Temperament& t, const NoteEventSet<> &events)
  {
    NoteEventScore score;
    if (events.Count() == 0)
      return score;

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

    const int quarterPulses = static_cast<int>(NoteValue::Quarter);
    const int longThreshold = quarterPulses;
    const int shortThreshold = static_cast<int>(NoteValue::Eighth);

    Note previousPitchedNote = REST;
    NoteValue previousPitchedDuration = NoteValue::None;
    bool inPhrase = false;

    for (size_t i = 0; i < events.Count(); i++)
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
          consonantPhraseStarts++;
        inPhrase = true;
      }

      if (eventPulses <= shortThreshold)
        shortPitchedEvents++;
      if (eventPulses >= longThreshold)
        longPitchedEvents++;

      if (previousPitchedNote != REST)
      {
        const int interval = t.GetNoteDistance(previousPitchedNote, events[i].note);
        if (events[i].note == previousPitchedNote)
          repeatedNotes++;
        else if (interval <= 2)
          stepMoves++;
        else if (interval >= 7)
        {
          if (interval >= 10)
            hugeLeaps++;
        }

        if (events[i].value == previousPitchedDuration)
          repeatedDurations++;

        transitionCount++;
      }

      const bool nextStartsNewPhrase =
          (i + 1 >= events.Count()) ||
          !events[i + 1].IsPitched() ||
          eventPulses >= longThreshold;

      if (nextStartsNewPhrase && t.IsConsonant(t.GetNoteDistance(0, events[i].note)))
        consonantPhraseEnds++;

      previousPitchedNote = events[i].note;
      previousPitchedDuration = events[i].value;
    }

    if (pitchedPulses == 0 || totalPulses == 0)
      return score;

    const int densityPct = static_cast<int>((pitchedPulses * 100UL) / totalPulses);
    const int restPct = static_cast<int>((restPulses * 100UL) / totalPulses);
    const int phraseStartPct = phraseCount > 0 ? static_cast<int>((consonantPhraseStarts * 100UL) / phraseCount) : 0;
    const int phraseEndPct = phraseCount > 0 ? static_cast<int>((consonantPhraseEnds * 100UL) / phraseCount) : 0;
    const int repeatPct = transitionCount > 0
                              ? static_cast<int>(((repeatedNotes + repeatedDurations) * 100UL) / (transitionCount * 2UL))
                              : 0;
    const int stepPct = transitionCount > 0 ? static_cast<int>((stepMoves * 100UL) / transitionCount) : 0;
    const int hugeLeapPct = transitionCount > 0 ? static_cast<int>((hugeLeaps * 100UL) / transitionCount) : 0;
    const int phraseDensity = pitchedEventCount > 0 ? static_cast<int>((phraseCount * 100UL) / pitchedEventCount) : 0;
    const int rhythmicContrast = (shortPitchedEvents + longPitchedEvents) > 0
                                     ? static_cast<int>((min(shortPitchedEvents, longPitchedEvents) * 200UL) /
                                                        (shortPitchedEvents + longPitchedEvents))
                                     : 0;

    score.density = clamp(100 - abs(densityPct - 55) * 2, 0, 100);
    score.rests = clamp(100 - abs(restPct - 25) * 2, 0, 100);
    score.cadence = clamp((phraseStartPct + (phraseEndPct * 2)) / 3, 0, 100);
    score.repetition = clamp(100 - abs(repeatPct - 30) * 2, 0, 100);
    score.contour = clamp(100 - abs(stepPct - 65) - (hugeLeapPct * 2), 0, 100);
    score.phrase = clamp(100 - abs(phraseDensity - 35) * 2, 0, 100);
    score.rhythm = clamp(rhythmicContrast, 0, 100);

    score.overall =
        (score.density * 20U +
         score.rests * 10U +
         score.cadence * 25U +
         score.repetition * 15U +
         score.contour * 15U +
         score.phrase * 10U +
         score.rhythm * 5U) /
        100U;

    return score;
  }

#if USE_DEBUG

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param ts
  /// @param granularity
  /// @param pattern
  void DebugPattern(const TimeSignature &ts, NoteValue granularity, PatternEventSet<> pattern)
  {
    if (pattern.Count() == 0 || granularity == NoteValue::None)
      return;

    int kPerBar = ts.beats * (static_cast<int>(ts.beatValue) /
                              static_cast<int>(granularity));
    for (size_t i = 0; i < pattern.Count(); i++)
    {
      if (i % kPerBar == 0)
        DPRINTF("|");
      if (pattern[i])
        DPRINTF("*");
      else
        DPRINTF("-");
    }
    DPRINTF("|\n");
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param t
  /// @param ts
  /// @param events
  void DebugNoteEvents(const Temperament &t,
                       const TimeSignature &ts,
                       const NoteEventSet<> &events)
  {
    int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int v = 0;
    char noteLabel[6];
    for (size_t i = 0; i < events.Count(); i++)
    {
      t.GetNoteLabel(events[i].note, noteLabel, sizeof(noteLabel));
      DPRINTF("%s(%d) ", noteLabel, static_cast<int>(events[i].value));

      v = events[i].value + v;
      if (v % vPerBar == 0)
        DPRINTF("\n");
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param t
  /// @param ts
  /// @param chords
  void DebugChordEvents(const Temperament &t,
                        const TimeSignature &ts,
                        const ChordEventSet<> chords)
  {
    int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int v = 0;
    char noteLabel[8];
    for (size_t i = 0; i < chords.Count(); i++)
    {
      t.GetNoteLabel(chords[i].root, noteLabel, sizeof(noteLabel));
      if (v % vPerBar == 0)
        DPRINTF("| ");
      DPRINTF("%s(%d)(%d) ", noteLabel, chords[i].root,
              static_cast<int>(chords[i].value));

      v = chords[i].value + v;
    }
    DPRINTF("|\n");
  }

#endif
} // namespace Music
