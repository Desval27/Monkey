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

namespace Music {
template<std::size_t MAX_EVENTS>
class RandomRandomPatternGenerator;

template<typename TRole, std::size_t MAX_EVENTS, typename = void>
struct PersonaPatternGeneratorSelector
{
  using Type = RandomRandomPatternGenerator<MAX_EVENTS>;
};

template<typename TRole, std::size_t MAX_EVENTS>
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
template<typename TRole,
         std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES,
         std::size_t MAX_EVENTS = DEF_MAX_EVENTS>
class Persona
{
public:
  using MySetup = Setup<MAX_DEGREES, SCALE_DEGREES>;
  using MyNoteGenerator =
    StyleANoteGenerator<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
  using DefaultPatternGenerator =
    typename PersonaPatternGeneratorSelector<TRole, MAX_EVENTS>::Type;

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param ts
  /// @param t
  /// @param s
  /// @param role
  Persona(const char* personaName, const MySetup& setup, const TRole& role)
    : personaName_(personaName)
    , setup_(&setup)
    , role_(role){};

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] const char* GetPersonaName() const
  {
    return personaName_.c_str();
  }

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  // const char *GetRoleName() const { return role_.Name; }

  [[nodiscard]] const char* GetRoleName() const { return role_.Name.c_str(); }

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param chords
  /// @param events
  /// @return
  std::size_t GenerateNoteEvents(
    const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords,
    NoteEventSet<MAX_EVENTS>& events)
  {
    return GenerateNoteEventsWithGenerator<DefaultPatternGenerator>(chords,
                                                                    events);
  }

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param chords
  /// @param events
  /// @return
  template<template<std::size_t> class TPatternGenerator>
  std::size_t GenerateNoteEvents(
    const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords,
    NoteEventSet<MAX_EVENTS>& events)
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
  template<typename TPatternGenerator>
  std::size_t GenerateNoteEventsWithGenerator(
    const ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords,
    NoteEventSet<MAX_EVENTS>& events)
  {
    const float density = GetDensity();
    const NoteValue granularity = GetGranularity();

    TPatternGenerator::GeneratePattern(
      setup_->timeSignature,
      chords.GetBarsForTimeSignature(setup_->timeSignature),
      density,
      granularity,
      pattern_);
    return MyNoteGenerator::GenerateEvents(
      *setup_, pattern_, chords, granularity, SCALE_WEIGHTS_7_UNIFORM, events);
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
  const MString<20> personaName_;
  const MySetup* setup_;
  const TRole& role_;
  PatternEventSet<MAX_EVENTS> pattern_;
};

} // namespace Music
