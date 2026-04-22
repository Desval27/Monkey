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

#include <type_traits>

namespace Music
{
  template <size_t MAX_EVENTS>
  class NullPatternGenerator;

  template <typename TRole, size_t MAX_EVENTS, typename = void>
  struct PersonaPatternGeneratorSelector
  {
    using Type = NullPatternGenerator<MAX_EVENTS>;
  };

  template <typename TRole, size_t MAX_EVENTS>
  struct PersonaPatternGeneratorSelector<
      TRole,
      MAX_EVENTS,
      std::void_t<typename TRole::template PatternGenerator<MAX_EVENTS>>>
  {
    using Type = typename TRole::template PatternGenerator<MAX_EVENTS>;
  };

  //////////////////////////////////////////////////////////////////////////
  /// @brief 
  /// @tparam TRole 
  /// @tparam MAX_EVENTS 
  template <typename TRole, size_t MAX_EVENTS = DEFAULT_MAX_EVENTS>
  class Persona
  {
  public:
    using DefaultPatternGenerator =
        typename PersonaPatternGeneratorSelector<TRole, MAX_EVENTS>::Type;

    //////////////////////////////////////////////////////////////////////////
    /// @brief 
    /// @param ts 
    /// @param t 
    /// @param s 
    /// @param role 
    Persona(const TimeSignature &ts, const Temperament &t, const ScaleMap &s,
            const TRole &role)
        : ts_(&ts), t_(&t), s_(&s), role_(role) {};

    //////////////////////////////////////////////////////////////////////////
    /// @brief 
    /// @return 
    const char *GetName() { return role_.Name; }

    //////////////////////////////////////////////////////////////////////////
    /// @brief 
    /// @param chords 
    /// @param events 
    /// @return 
    size_t GenerateNoteEvents(const ChordEventSet<MAX_EVENTS> &chords,
                              NoteEventSet<MAX_EVENTS> &events)
    {
      return GenerateNoteEventsWithGenerator<DefaultPatternGenerator>(chords,
                                                                      events);
    }

    //////////////////////////////////////////////////////////////////////////
    /// @brief 
    /// @param chords 
    /// @param events 
    /// @return 
    template <template <size_t> class TPatternGenerator>
    size_t GenerateNoteEvents(const ChordEventSet<MAX_EVENTS> &chords,
                              NoteEventSet<MAX_EVENTS> &events)
    {
      return GenerateNoteEventsWithGenerator<TPatternGenerator<MAX_EVENTS>>(
          chords, events);
    }

    //////////////////////////////////////////////////////////////////////////
    /// @brief 
    /// @tparam TPatternGenerator 
    /// @param chords 
    /// @param events 
    /// @return 
    template <typename TPatternGenerator>
    size_t GenerateNoteEventsWithGenerator(
        const ChordEventSet<MAX_EVENTS> &chords,
        NoteEventSet<MAX_EVENTS> &events)
    {
      PatternEventSet<MAX_EVENTS> pattern;
      const float density = GetDensity();
      const NoteValue granularity = GetGranularity();

      TPatternGenerator::GeneratePattern(*ts_,
                                         chords.GetBarsForTimeSignature(*ts_),
                                         density,
                                         granularity,
                                         pattern);
      events.Clear();
      return events.Count();                                                        
    }

    //////////////////////////////////////////////////////////////////////////
    /// @brief 
    /// @return 
    float GetDensity() { return randomRange(role_.density); }

    //////////////////////////////////////////////////////////////////////////
    /// @brief 
    /// @return 
    NoteValue GetGranularity() { return role_.granularity; }

  private:
    const TimeSignature *ts_;
    const Temperament *t_;
    const ScaleMap *s_;
    const TRole &role_;
  };

} // namespace Music
