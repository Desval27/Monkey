/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Debug.cpp
 * @brief Sometimes helpful debug routines.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#include <Music/Music.h>

#ifndef DAISY_PLATFORM
#include <TTY.h>
#include <iostream>
#include <iomanip>
#endif

namespace Music 
{
#ifndef DAISY_PLATFORM

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param ts
  /// @param granularity
  /// @param pattern
  void DebugPattern(const TimeSignature &ts, NoteValue granularity, PatternEventSet<> pattern)
  {
    std::cout << TTY_FG_YELLOW << "PATTERN EVENTS" << TTY_RESET << " Density: " << pattern.GetDensity() << std::endl;
    if (pattern.Count() == 0 || granularity == NoteValue::None)
      return;

    const int g = static_cast<int>(granularity);
    const int kPerBar = (ts.beats * static_cast<int>(ts.beatValue)) / g;
    if (kPerBar > 0) 
    {
      for (size_t i = 0; i < pattern.Count(); i++)
      {      
        if (i % kPerBar == 0)
          std::cout << "|";
        if (pattern[i])
          std::cout << "*";
        else
          std::cout << "-";
      }
    }
    std::cout << "|" << std::endl;
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
    std::cout << TTY_FG_CYAN << "NOTE EVENTS" << TTY_RESET << " Density: " << events.GetDensity() << std::endl;

    int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int v = 0;
    char noteLabel[8];
    for (size_t i = 0; i < events.Count(); i++)
    {
      if (!events[i].IsPitched())
        std::cout << TTY_FAINT;

      t.GetNoteLabel(events[i].note, noteLabel, sizeof(noteLabel));

      std::cout << std::setw(5) << std::right << noteLabel << "-" << std::setw(10) << std::left << GetNoteValueText(events[i].value);

      if (!events[i].IsPitched())
        std::cout << TTY_RESET;

      v = events[i].value + v;
      if (v % vPerBar == 0)
        std::cout << std::endl;
    }
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param t
  /// @param ts
  /// @param chords
  template <std::size_t SCALE_MAP_DEGREES, std::size_t MAX_EVENTS>
  void DebugChordEvents(const TimeSignature &ts, const Temperament &t, const ScaleMap<SCALE_MAP_DEGREES> &s, const ChordEventSet<SCALE_MAP_DEGREES, MAX_EVENTS> chords)
  {
    int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int v = 0;
    char noteLabel[8];
    std::cout << TTY_FG_GREEN << "CHORD EVENTS" << TTY_RESET << " Density: " << chords.GetDensity() << std::endl;
    for (size_t i = 0; i < chords.Count(); i++)
    {
      Note tones[8];
      size_t toneCount = chords[i].GetChordTones(
          s, static_cast<int>(t.DegreesPerPeriod()), tones, ArrayLen(tones));

      if (v % vPerBar == 0)
        std::cout << "| ";
      std::cout << chords[i].GetChordName(s, t) << " (";

      for (size_t j = 0; j < toneCount; j++)
      {
        t.GetNoteLabel(tones[j], noteLabel, sizeof(noteLabel));
        std::cout << noteLabel;
        if (j < toneCount - 1)
          std::cout << "|";
      }

      std::cout << ")-" << GetNoteValueText(chords[i].value) << " ";

      v = chords[i].value + v;
    }
    std::cout << "|" << std::endl;
  }

  template void DebugChordEvents<HEPATONIC, DEFAULT_MAX_EVENTS>(
      const TimeSignature &ts,
      const Temperament &t,
      const ScaleMap<HEPATONIC> &s,
      const ChordEventSet<HEPATONIC, DEFAULT_MAX_EVENTS> chords);

#endif
} // namespace Music
