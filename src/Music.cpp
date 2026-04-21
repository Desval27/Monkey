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

    int n = (ts.beats * bars) *
            (static_cast<int>(ts.beatValue) / static_cast<int>(granularity));
    int k = n * density;
    return BuildEuclid(k, n, 1, pattern);
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
