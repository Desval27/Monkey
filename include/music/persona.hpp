/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file persona.hpp
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <type_traits>

#include <monkey.hpp>
#include <music/event_set.hpp>
#include <music/pattern_generator.hpp>
#include <music/scale_map.hpp>
#include <music/temperament.hpp>
#include <music/time_signature.hpp>

namespace music {

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

#define TESTING_WEIGHTS SCALE_WEIGHTS_7_UNIFORM

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
    , role_(role) {};

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
  /// @param granularity
  /// @param chords
  /// @return
  std::size_t GenerateChordEvents(
    ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>& chords)
  {
    const NoteValue granularity = get_granularity();
    return GenerateStandardChordEvents(*setup_, granularity, chords);
  }

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
    const float density = get_density();
    const NoteValue granularity = get_granularity();

    TPatternGenerator::generate_pattern(
      setup_->timeSignature,
      chords.get_bars_for_time_signature(setup_->timeSignature),
      density,
      granularity,
      pattern_);
    MyNoteGenerator::generate_events(
      *setup_, pattern_, chords, granularity, GetWeightMap(), events);

    // Likelyhood of haveing it repeat sections
    if (randomRange(0.0F, 1.0F) < get_repeat_probability()) {
      return PatternModifier<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>::
        repeat_two(*setup_, events);
    }
    return events.size();
  }

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] float get_density() const { return randomRange(role_.density); }

  [[nodiscard]] float get_repeat_probability() const
  {
    return randomRange(role_.repeat_probability);
  }

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] NoteValue get_granularity() const { return role_.granularity; }

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  const WeightMap<SCALE_DEGREES>& GetWeightMap() { return role_.weight_map; }

private:
  const MString<20> personaName_;
  const MySetup* setup_;
  const TRole& role_;
  PatternEventSet<MAX_EVENTS> pattern_;
};

} // namespace Music
