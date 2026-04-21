/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Persona.h
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Monkey.h>
#include <Music/EventSet.h>
#include <Music/ScaleMap.h>
#include <Music/Temperament.h>
#include <Music/TimeSignature.h>

namespace Music
{
  template <size_t MAX_EVENTS>
  class NullGenerator;

  template <typename TRole, size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  class Persona
  {
  public:
    Persona(const TimeSignature &ts, const Temperament &t, const ScaleMap &s,
            const TRole &role)
        : ts_(&ts), t_(&t), s_(&s), role_(role) {};

    const char *GetName() { return role_.Name; }

    template <template <size_t> class TPatternGenerator = NullGenerator>
    size_t GenerateNoteEvents(const ChordEventSet<MAX_EVENTS> &chords,
                              NoteEventSet<MAX_EVENTS> &events)
    {
      PatternEventSet<MAX_EVENTS> pattern;
      const float density = GetDensity();
      const NoteValue granularity = GetGranularity();

      TPatternGenerator<MAX_EVENTS>::GeneratePattern(*ts_,
                                                     chords.GetBarsForTimeSignature(*ts_), 
                                                     GetDensity(),
                                                     GetGranularity(), 
                                                     pattern);
      events.Clear();
      return events.Count();                                                        
    }

    float GetDensity() { return randomRange(role_.density); }
    NoteValue GetGranularity() { return role_.granularity; }

  private:
    const TimeSignature *ts_;
    const Temperament *t_;
    const ScaleMap *s_;
    const TRole &role_;
  };

} // namespace Music
