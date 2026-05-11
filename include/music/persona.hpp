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

#include <Music/event_set.hpp>
#include <Music/pattern_generator.hpp>
#include <Music/scale_map.hpp>
#include <Music/temperament.hpp>
#include <Music/time_signature.hpp>
#include <monkey.hpp>

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

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @tparam SCALE_DEGREES
template<std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES>
  requires(SCALE_DEGREES >= PENTATONIC && SCALE_DEGREES <= HEPATONIC)
struct StockPersonaRole
{
  const MString<10> label = "STOCK ROLE";
  const NoteValue granularity = NoteValue::Quarter;
  const int period_offset = 0;
  const Range<float> density{ 0.0F, ALMOST_ONE };
  const Range<float> repeat_probability{ 0.0F, 0.0F };
  const WeightMap<SCALE_DEGREES>& weight_map;

  /////////////////////////////////////////////////////////////////////////////
  StockPersonaRole(NoteValue granularity = NoteValue::Whole)
    : granularity(granularity)
    , period_offset(0)
    , density(0.0, ALMOST_ONE)
    , repeat_probability(0.0, ALMOST_ONE)
    , weight_map(get_default_weight_map())
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param granularity
  /// @param period_offset
  /// @param density_low
  /// @param density_high
  /// @param repeat_low
  /// @param repeat_high
  StockPersonaRole(NoteValue granularity,
                   int period_offset,
                   float density_low,
                   float density_high,
                   float repeat_low = 0.0F,
                   float repeat_high = 0.0F)
    : granularity(granularity)
    , period_offset(period_offset)
    , density(density_low, density_high)
    , repeat_probability(repeat_low, repeat_high)
    , weight_map(get_default_weight_map())
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param granularity
  /// @param period_offset
  /// @param density_low
  /// @param density_high
  /// @param weight_map
  /// @param repeat_low
  /// @param repeat_high
  StockPersonaRole(NoteValue granularity,
                   int period_offset,
                   const WeightMap<SCALE_DEGREES>& weight_map,
                   float density_low,
                   float density_high,
                   float repeat_low = 0.0F,
                   float repeat_high = 0.0F)
    : granularity(granularity)
    , period_offset(period_offset)
    , density(density_low, density_high)
    , repeat_probability(repeat_low, repeat_high)
    , weight_map(weight_map)
  {
  }

  static const WeightMap<SCALE_DEGREES>& get_default_weight_map()
  {
    if constexpr (SCALE_DEGREES == PENTATONIC) {
      return SCALE_WEIGHTS_5_UNIFORM;
    } else if constexpr (SCALE_DEGREES == HEXATONIC) {
      return SCALE_WEIGHTS_6_UNIFORM;
    } else if constexpr (SCALE_DEGREES == HEPATONIC) {
      return SCALE_WEIGHTS_7_UNIFORM;
    }
  }
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
  using TSetup = Setup<MAX_DEGREES, SCALE_DEGREES>;
  using TNoteGenerator =
    StyleANoteGenerator<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
  using TDefaultPatternGenerator =
    typename PersonaPatternGeneratorSelector<TRole, MAX_EVENTS>::Type;

  //////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param ts
  /// @param t
  /// @param s
  /// @param role
  Persona(const char* personaName, const TSetup& setup, const TRole& role)
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

  [[nodiscard]] const char* GetRoleName() const { return role_.label.c_str(); }

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
    return GenerateNoteEventsWithGenerator<TDefaultPatternGenerator>(chords,
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
      setup_->time_signature,
      chords.get_bars_for_time_signature(setup_->time_signature),
      density,
      granularity,
      pattern_);
    TNoteGenerator::generate_events(
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
  const TSetup* setup_;
  const TRole& role_;
  PatternEventSet<MAX_EVENTS> pattern_;
};

} // namespace Music
