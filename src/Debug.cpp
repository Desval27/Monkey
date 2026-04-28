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
  template <std::size_t MAX_EVENTS>
  void DebugPattern(const TimeSignature &ts, NoteValue granularity, const PatternEventSet<MAX_EVENTS> &pattern)
  {
    std::cout << TTY_FG_YELLOW << "PATTERN EVENTS" << TTY_RESET << " Density: " << pattern.GetDensity() << std::endl;
    if (pattern.Count() == 0 || granularity == NoteValue::None)
      return;

    const int g = static_cast<int>(granularity);
    const int kPerBar = (ts.beats * static_cast<int>(ts.beatValue)) / g;
    if (kPerBar > 0) 
    {
      for (std::size_t i = 0; i < pattern.Count(); i++)
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
  template <std::size_t MAX_DEGREES, std::size_t MAX_EVENTS>
  void DebugNoteEvents(const Temperament<MAX_DEGREES> &t,
                       const TimeSignature &ts,
                       const NoteEventSet<MAX_EVENTS> &events)
  {
    std::cout << TTY_FG_CYAN << "NOTE EVENTS" << TTY_RESET << " Density: " << events.GetDensity() << std::endl;

    int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int v = 0;
    char noteLabel[8];
    for (std::size_t i = 0; i < events.Count(); i++)
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
  template <std::size_t MAX_DEGREES, std::size_t SCALE_DEGREES, std::size_t MAX_EVENTS>
  void DebugChordEvents(const TimeSignature &ts, const Temperament<MAX_DEGREES> &t, const ScaleMap<SCALE_DEGREES> &s, const ChordEventSet<SCALE_DEGREES, MAX_EVENTS> &chords)
  {
    int vPerBar = ts.beats * static_cast<int>(ts.beatValue);
    int v = 0;
    char noteLabel[8];
    MString<16> chordName;
    std::cout << TTY_FG_GREEN << "CHORD EVENTS" << TTY_RESET << " Density: " << chords.GetDensity() << std::endl;
    for (std::size_t i = 0; i < chords.Count(); i++)
    {
      Note tones[8];
      std::size_t toneCount = chords[i].GetChordTones(
          s, static_cast<int>(t.DegreesPerPeriod()), tones, ArrayLen(tones));

      if (v % vPerBar == 0)
        std::cout << "| ";
      chords[i].GetChordName(s, t, chordName);
      std::cout << chordName << " (";

      for (std::size_t j = 0; j < toneCount; j++)
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

  template void DebugPattern<DEF_MAX_EVENTS>(
      const TimeSignature &ts,
      NoteValue granularity,
      const PatternEventSet<DEF_MAX_EVENTS> &pattern);

  template void DebugNoteEvents<DEF_MAX_DEGREES, DEF_MAX_EVENTS>(
      const Temperament<DEF_MAX_DEGREES> &t,
      const TimeSignature &ts,
      const NoteEventSet<DEF_MAX_EVENTS> &events);

  template void DebugChordEvents<DEF_MAX_DEGREES, DEF_SCALE_DEGREES, DEF_MAX_EVENTS>(
      const TimeSignature &ts,
      const Temperament<DEF_MAX_DEGREES> &t,
      const ScaleMap<DEF_SCALE_DEGREES> &s,
      const ChordEventSet<DEF_SCALE_DEGREES, DEF_MAX_EVENTS> &chords);

#endif
} // namespace Music
