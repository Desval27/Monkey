/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Chord.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief Chord definitions and voicing.
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include <music/note_value.hpp>
#include <music/scale_map.hpp>
#include <music/temperament.hpp>

namespace music {

constexpr int SCALE_CHORD_COUNT = 7;
constexpr std::size_t DEFAULT_CHORD_TONE_COUNT = 3;

///////////////////////////////////////////////////////////////////////////////
/// @brief
enum class ChordExtension : uint8_t
{
  None = 0,
  Sixth = 1 << 0,
  Seventh = 1 << 1,
  Ninth = 1 << 2,
  Eleventh = 1 << 3,
  Thirteenth = 1 << 4,
  Sus2 = 1 << 5,
  Sus4 = 1 << 6,
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
enum class ChordAlteration : uint8_t
{
  None = 0,
  Flat5 = 1 << 0,
  Sharp5 = 1 << 1,
  Flat9 = 1 << 2,
  Sharp9 = 1 << 3,
  Sharp11 = 1 << 4,
  Flat13 = 1 << 5,
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param lhs
/// @param rhs
/// @return
static inline ChordExtension
operator|(ChordExtension lhs, ChordExtension rhs)
{
  return static_cast<ChordExtension>(static_cast<uint16_t>(lhs) |
                                     static_cast<uint16_t>(rhs));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param lhs
/// @param rhs
/// @return
static inline ChordExtension
operator&(ChordExtension lhs, ChordExtension rhs)
{
  return static_cast<ChordExtension>(static_cast<uint16_t>(lhs) &
                                     static_cast<uint16_t>(rhs));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param lhs
/// @param rhs
/// @return
static inline ChordExtension&
operator|=(ChordExtension& lhs, ChordExtension rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param lhs
/// @param rhs
/// @return
static inline ChordAlteration
operator|(ChordAlteration lhs, ChordAlteration rhs)
{
  return static_cast<ChordAlteration>(static_cast<uint16_t>(lhs) |
                                      static_cast<uint16_t>(rhs));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param lhs
/// @param rhs
/// @return
static inline ChordAlteration
operator&(ChordAlteration lhs, ChordAlteration rhs)
{
  return static_cast<ChordAlteration>(static_cast<uint16_t>(lhs) &
                                      static_cast<uint16_t>(rhs));
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param lhs
/// @param rhs
/// @return
static inline ChordAlteration&
operator|=(ChordAlteration& lhs, ChordAlteration rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param value
/// @param flag
/// @return
static inline bool
has_flag(ChordExtension value, ChordExtension flag)
{
  return (static_cast<uint16_t>(value & flag) != 0);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param value
/// @param flag
/// @return
static inline bool
has_flag(ChordAlteration value, ChordAlteration flag)
{
  return (static_cast<uint16_t>(value & flag) != 0);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Represents a chord event in the musical context.  Includes the
/// chord's root note, duration, extensions, alterations, and inversion.
/// @tparam MAX_DEGREES
/// @tparam SCALE_DEGREES
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         std::size_t SCALE_DEGREES = DEF_SCALE_DEGREES>
struct ChordEvent
{
  ChordEvent()
    : root(0)
    , value(NoteValue::None)
    , extensions(ChordExtension{})
    , alterations(ChordAlteration{})
    , inversion(0)
  {
  }

  ChordEvent(Note r,
             NoteValue v,
             ChordExtension ext = ChordExtension{},
             ChordAlteration alt = ChordAlteration{},
             int inv = 0)
    : root(r)
    , value(v)
    , extensions(ext)
    , alterations(alt)
    , inversion(inv)
  {
  }

  Note root;
  NoteValue value;
  ChordExtension extensions;
  ChordAlteration alterations;
  int inversion;

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param scale
  /// @param notes
  /// @return
  template<std::size_t N>
  std::size_t get_chord_tones(const ScaleMap<SCALE_DEGREES>& scale,
                              std::array<Note, N>& notes) const
  {
    return get_chord_tones(scale, TWELVE_TONE, notes);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param scale
  /// @param degrees_per_period
  /// @param notes
  /// @return
  template<std::size_t N>
  std::size_t get_chord_tones(const ScaleMap<SCALE_DEGREES>& scale,
                              int degrees_per_period,
                              std::array<Note, N>& notes) const
  {
    if (notes.size() == 0 || scale.size() == 0 || degrees_per_period <= 0) {
      return 0;
    }

    const std::size_t root_index = scale.get_index_of_degree(root);
    if (root_index >= scale.size()) {
      return 0;
    }

    int root_period_offset = 0;
    if (scale.get_mapped_degree(root_index, root_period_offset) != root) {
      return 0;
    }

    const int third_span = has_flag(extensions, ChordExtension::Sus4)   ? 3
                           : has_flag(extensions, ChordExtension::Sus2) ? 1
                                                                        : 2;

    std::size_t tone_count = 0;
    add_scale_tone(
      scale, root_index, 0, 0, degrees_per_period, tone_count, notes);
    add_scale_tone(
      scale, root_index, third_span, 0, degrees_per_period, tone_count, notes);
    add_scale_tone(scale,
                   root_index,
                   4,
                   fifth_alteration(),
                   degrees_per_period,
                   tone_count,
                   notes);

    if (has_flag(extensions, ChordExtension::Sixth)) {
      add_scale_tone(
        scale, root_index, 5, 0, degrees_per_period, tone_count, notes);
    }
    if (has_flag(extensions, ChordExtension::Seventh)) {
      add_scale_tone(
        scale, root_index, 6, 0, degrees_per_period, tone_count, notes);
    }
    if (has_flag(extensions, ChordExtension::Ninth)) {
      add_scale_tone(scale,
                     root_index,
                     8,
                     ninth_alteration(),
                     degrees_per_period,
                     tone_count,
                     notes);
    }
    if (has_flag(extensions, ChordExtension::Eleventh)) {
      add_scale_tone(scale,
                     root_index,
                     10,
                     eleventh_alteration(),
                     degrees_per_period,
                     tone_count,
                     notes);
    }
    if (has_flag(extensions, ChordExtension::Thirteenth)) {
      add_scale_tone(scale,
                     root_index,
                     12,
                     thirteenth_alteration(),
                     degrees_per_period,
                     tone_count,
                     notes);
    }

    apply_inversion(notes, tone_count, degrees_per_period);

    return tone_count;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam N
  /// @param scale
  /// @param out
  /// @param degrees_per_period
  /// @return
  template<std::size_t N>
  bool get_chord_name(const ScaleMap<SCALE_DEGREES>& scale,
                      MString<N>& out,
                      int degrees_per_period = TWELVE_TONE) const
  {
    const std::size_t root_index = scale.get_index_of_degree(root);
    if (scale.size() == 0 || root_index >= scale.size()) {
      return out.set("?");
    }

    int period_offset = 0;
    if (scale.get_mapped_degree(root_index, period_offset) != root) {
      return out.set("?");
    }

    std::array<Note, 8> tones;
    const std::size_t tone_count =
      get_chord_tones(scale, degrees_per_period, tones);
    return build_chord_name_from_tones(scale,
                                       root_index,
                                       root,
                                       tones,
                                       tone_count,
                                       degrees_per_period,
                                       extensions,
                                       alterations,
                                       out);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam TEMPERAMENT_DEGREES
  /// @tparam N
  /// @param scale
  /// @param temperament
  /// @param out
  /// @return
  template<std::size_t TEMPERAMENT_DEGREES, std::size_t N>
  bool get_chord_name(const ScaleMap<SCALE_DEGREES>& scale,
                      const Temperament<TEMPERAMENT_DEGREES>& temperament,
                      MString<N>& out) const
  {
    const std::size_t root_index = scale.get_index_of_degree(root);
    if (scale.size() == 0 || root_index >= scale.size()) {
      return out.set("?");
    }

    int period_offset = 0;
    if (scale.get_mapped_degree(root_index, period_offset) != root) {
      return out.set("?");
    }

    std::array<Note, 8> tones;
    const std::size_t tone_count = get_chord_tones(
      scale, static_cast<int>(temperament.degrees_per_period()), tones);
    if (tone_count < 3) {
      return out.set("?");
    }

    MString<8> third_label;
    MString<8> fifth_label;
    Note quality_fifth = tones[2];
    if (has_flag(alterations, ChordAlteration::Flat5)) {
      quality_fifth = static_cast<Note>(quality_fifth + 1);
    } else if (has_flag(alterations, ChordAlteration::Sharp5)) {
      quality_fifth = static_cast<Note>(quality_fifth - 1);
    }

    if (!temperament.get_interval_label(
          static_cast<Degree>(
            wrap(static_cast<int>(tones[1] - root),
                 static_cast<int>(temperament.degrees_per_period()))),
          third_label) ||
        !temperament.get_interval_label(
          static_cast<Degree>(
            wrap(static_cast<int>(quality_fifth - root),
                 static_cast<int>(temperament.degrees_per_period()))),
          fifth_label)) {
      return get_chord_name(
        scale, out, static_cast<int>(temperament.degrees_per_period()));
    }

    ChordQuality quality =
      has_flag(extensions, ChordExtension::Sus4) ? ChordQuality::Suspended4
      : has_flag(extensions, ChordExtension::Sus2)
        ? ChordQuality::Suspended2
        : classify_chord_quality_from_interval_labels(third_label, fifth_label);
    if (quality == ChordQuality::Unknown) {
      const int lower_third =
        wrap(static_cast<int>(tones[1] - root),
             static_cast<int>(temperament.degrees_per_period()));
      const int upper_third =
        wrap(static_cast<int>(tones[2] - tones[1]),
             static_cast<int>(temperament.degrees_per_period()));

      std::array<int, SCALE_CHORD_COUNT> all_thirds = {};
      std::size_t third_count = 0;
      for (std::size_t i = 0; i < scale.size() && i < SCALE_CHORD_COUNT; ++i) {
        int first_period = 0;
        int third_period = 0;
        const Degree first = scale.get_mapped_degree(i, first_period);
        const Degree third = scale.get_mapped_degree(i + 2, third_period);
        all_thirds[third_count++] =
          (third + (third_period *
                    static_cast<int>(temperament.degrees_per_period()))) -
          (first +
           (first_period * static_cast<int>(temperament.degrees_per_period())));
      }

      quality = classify_chord_quality_from_third_stack(
        lower_third,
        upper_third,
        min_value(all_thirds, third_count),
        max_value(all_thirds, third_count));
    }

    if (!build_chord_name_from_quality(root_index, quality, out)) {
      return false;
    }
    return append_extension_name(out, extensions) &&
           append_alteration_name(out, alterations);
  }

private:
  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  enum class ChordQuality : uint8_t
  {
    Unknown,
    Major,
    Minor,
    Diminished,
    Augmented,
    Suspended2,
    Suspended4,
    PowerChord,
  };

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam N
  /// @param scale
  /// @param scale_index
  /// @param chord_root
  /// @param tones
  /// @param tone_count
  /// @param degrees_per_period
  /// @param extensions
  /// @param alterations
  /// @param out
  /// @return
  template<std::size_t N1, std::size_t N2>
  static bool build_chord_name_from_tones(const ScaleMap<SCALE_DEGREES>& scale,
                                          int scale_index,
                                          Note chord_root,
                                          const std::array<Note, N1>& tones,
                                          std::size_t tone_count,
                                          int degrees_per_period,
                                          ChordExtension extensions,
                                          ChordAlteration alterations,
                                          MString<N2>& out)
  {
    std::array<Note, 3> quality_tones = {};
    if (tone_count >= 3) {
      for (std::size_t i = 0; i < 3; ++i) {
        quality_tones[i] = tones[i];
      }

      if (has_flag(alterations, ChordAlteration::Flat5)) {
        quality_tones[2] = static_cast<Note>(quality_tones[2] + 1);
      } else if (has_flag(alterations, ChordAlteration::Sharp5)) {
        quality_tones[2] = static_cast<Note>(quality_tones[2] - 1);
      }
    }

    const ChordQuality quality =
      has_flag(extensions, ChordExtension::Sus4)   ? ChordQuality::Suspended4
      : has_flag(extensions, ChordExtension::Sus2) ? ChordQuality::Suspended2
      : tone_count >= 3 ? classify_chord_quality(scale,
                                                 scale_index,
                                                 chord_root,
                                                 quality_tones,
                                                 quality_tones.size(),
                                                 degrees_per_period)
                        : classify_chord_quality(scale,
                                                 scale_index,
                                                 chord_root,
                                                 tones,
                                                 tone_count,
                                                 degrees_per_period);

    switch (quality) {
      default: {
        if (!build_chord_name_from_quality(scale_index, quality, out)) {
          return false;
        }
        return append_extension_name(out, extensions) &&
               append_alteration_name(out, alterations);
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] int fifth_alteration() const
  {
    if (has_flag(alterations, ChordAlteration::Flat5)) {
      return -1;
    }
    if (has_flag(alterations, ChordAlteration::Sharp5)) {
      return 1;
    }
    return 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] int ninth_alteration() const
  {
    if (has_flag(alterations, ChordAlteration::Flat9)) {
      return -1;
    }
    if (has_flag(alterations, ChordAlteration::Sharp9)) {
      return 1;
    }
    return 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] int eleventh_alteration() const
  {
    return has_flag(alterations, ChordAlteration::Sharp11) ? 1 : 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @return
  [[nodiscard]] int thirteenth_alteration() const
  {
    return has_flag(alterations, ChordAlteration::Flat13) ? -1 : 0;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param notes
  /// @param tone_count
  /// @param degrees_per_period
  template<std::size_t N>
  void apply_inversion(std::array<Note, N>& notes,
                       std::size_t tone_count,
                       int degrees_per_period) const
  {
    tone_count = std::min(tone_count, notes.size());
    if (tone_count < 2 || inversion == 0) {
      return;
    }

    if (inversion > 0) {
      const std::size_t turns =
        static_cast<std::size_t>(inversion) % tone_count;
      for (std::size_t i = 0; i < turns; ++i) {
        const Note moved = static_cast<Note>(notes[0] + degrees_per_period);
        for (std::size_t j = 1; j < tone_count; ++j) {
          notes[j - 1] = notes[j];
        }
        notes[tone_count - 1] = moved;
      }
      return;
    }

    const std::size_t turns = static_cast<std::size_t>(-inversion) % tone_count;
    for (std::size_t i = 0; i < turns; ++i) {
      const Note moved =
        static_cast<Note>(notes[tone_count - 1] - degrees_per_period);
      for (std::size_t j = tone_count - 1; j > 0; --j) {
        notes[j] = notes[j - 1];
      }
      notes[0] = moved;
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param scale
  /// @param root_index
  /// @param scale_span
  /// @param alteration
  /// @param degrees_per_period
  /// @param tone_count
  /// @param notes
  template<std::size_t N>
  static void add_scale_tone(const ScaleMap<SCALE_DEGREES>& scale,
                             std::size_t root_index,
                             int scale_span,
                             int alteration,
                             int degrees_per_period,
                             std::size_t& tone_count,
                             std::array<Note, N>& notes)
  {
    if (tone_count >= notes.size()) {
      return;
    }

    int period_offset = 0;
    const Note mapped = static_cast<Note>(
      scale.get_mapped_degree(root_index + scale_span, period_offset));
    notes[tone_count++] = static_cast<Note>(
      mapped + (period_offset * degrees_per_period) + alteration);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam N
  /// @param scale
  /// @param scale_index
  /// @param chord_root
  /// @param tones
  /// @param tone_count
  /// @param degrees_per_period
  /// @return
  template<std::size_t N>
  static ChordQuality classify_chord_quality(
    const ScaleMap<SCALE_DEGREES>& scale,
    int scale_index,
    Note chord_root,
    const std::array<Note, N>& tones,
    std::size_t tone_count,
    int degrees_per_period)
  {
    tone_count = std::min(tone_count, tones.size());
    if (tone_count < 3 || degrees_per_period <= 0 || scale.size() < 3) {
      return ChordQuality::Unknown;
    }

    std::array<int, SCALE_CHORD_COUNT> third_intervals = {};
    std::array<int, SCALE_CHORD_COUNT> fifth_intervals = {};
    std::size_t third_count = 0;
    std::size_t fifth_count = 0;

    for (std::size_t i = 0; i < scale.size() && i < SCALE_CHORD_COUNT; ++i) {
      third_intervals[third_count++] =
        get_scale_span_interval(scale, i, 2, degrees_per_period);
      fifth_intervals[fifth_count++] =
        get_scale_span_interval(scale, i, 4, degrees_per_period);
    }

    const int minor_third = min_value(third_intervals, third_count);
    const int major_third = max_value(third_intervals, third_count);

    const int triad_third =
      wrap(static_cast<int>(tones[1] - chord_root), degrees_per_period);
    const int triad_fifth =
      wrap(static_cast<int>(tones[2] - chord_root), degrees_per_period);

    const int diminished_fifth =
      greatest_value_less_than(fifth_intervals, fifth_count, triad_fifth);
    const int augmented_fifth =
      least_value_greater_than(fifth_intervals, fifth_count, triad_fifth);

    int perfect_fifth = triad_fifth;
    bool found_perfect_fifth = false;
    for (std::size_t i = 0; i < fifth_count; ++i) {
      const int candidate = fifth_intervals[i];
      if (candidate > diminished_fifth &&
          (!found_perfect_fifth || candidate < perfect_fifth)) {
        perfect_fifth = candidate;
        found_perfect_fifth = true;
      }
    }

    if (triad_third == major_third && triad_fifth == perfect_fifth) {
      return ChordQuality::Major;
    }
    if (triad_third == minor_third && triad_fifth == perfect_fifth) {
      return ChordQuality::Minor;
    }
    if (triad_third == minor_third && triad_fifth == diminished_fifth) {
      return ChordQuality::Diminished;
    }
    if (triad_third == major_third && triad_fifth == augmented_fifth) {
      return ChordQuality::Augmented;
    }

    const int upper_third =
      wrap(static_cast<int>(tones[2] - tones[1]), degrees_per_period);
    (void)scale_index;
    return classify_chord_quality_from_third_stack(
      triad_third, upper_third, minor_third, major_third);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param third_label
  /// @param fifth_label
  /// @return
  static ChordQuality classify_chord_quality_from_interval_labels(
    const char* third_label,
    const char* fifth_label)
  {
    if ((third_label == nullptr) || (fifth_label == nullptr)) {
      return ChordQuality::Unknown;
    }

    if (matches_interval_label(third_label, "M3") &&
        matches_interval_label(fifth_label, "P5")) {
      return ChordQuality::Major;
    }
    if (matches_interval_label(third_label, "m3") &&
        matches_interval_label(fifth_label, "P5")) {
      return ChordQuality::Minor;
    }
    if (matches_interval_label(third_label, "m3") &&
        matches_interval_label(fifth_label, "d5")) {
      return ChordQuality::Diminished;
    }
    if (matches_interval_label(third_label, "M3") &&
        matches_interval_label(fifth_label, "A5")) {
      return ChordQuality::Augmented;
    }
    if (matches_interval_label(third_label, "M2") &&
        matches_interval_label(fifth_label, "P5")) {
      return ChordQuality::Suspended2;
    }
    if (matches_interval_label(third_label, "P4") &&
        matches_interval_label(fifth_label, "P5")) {
      return ChordQuality::Suspended4;
    }
    return ChordQuality::Unknown;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param lower_third
  /// @param upper_third
  /// @param smallThird
  /// @param largeThird
  /// @return
  static ChordQuality classify_chord_quality_from_third_stack(int lower_third,
                                                              int upper_third,
                                                              int smallThird,
                                                              int largeThird)
  {
    if (lower_third > upper_third) {
      return ChordQuality::Major;
    }
    if (lower_third < upper_third) {
      return ChordQuality::Minor;
    }

    const int midpoint = smallThird + ((largeThird - smallThird) / 2);
    if (lower_third <= midpoint) {
      return ChordQuality::Diminished;
    }
    return ChordQuality::Augmented;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam N
  /// @param scale_index
  /// @param quality
  /// @param out
  /// @return
  template<std::size_t N>
  static bool build_chord_name_from_quality(int scale_index,
                                            ChordQuality quality,
                                            MString<N>& out)
  {
    switch (quality) {
      case ChordQuality::Major:
        return out.set(roman_numeral_for_scale_index(scale_index));
      case ChordQuality::Minor:
        return out.set(lowercase_roman_numeral_for_scale_index(scale_index));
      case ChordQuality::Diminished:
        return out.set(lowercase_roman_numeral_for_scale_index(scale_index)) &&
               out.append("_o");
      case ChordQuality::Augmented:
        return out.set(roman_numeral_for_scale_index(scale_index)) &&
               out.append("+");
      case ChordQuality::Suspended2:
        return out.set(roman_numeral_for_scale_index(scale_index)) &&
               out.append("sus2");
      case ChordQuality::Suspended4:
        return out.set(roman_numeral_for_scale_index(scale_index)) &&
               out.append("sus4");
      case ChordQuality::PowerChord:
        return out.set(roman_numeral_for_scale_index(scale_index)) &&
               out.append("5");
      case ChordQuality::Unknown:
      default:
        return out.set(roman_numeral_for_scale_index(scale_index)) &&
               out.append("?");
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam N
  /// @param name
  /// @param extensions
  /// @return
  template<std::size_t N>
  static bool append_extension_name(MString<N>& name, ChordExtension extensions)
  {
    if (has_flag(extensions, ChordExtension::Thirteenth)) {
      return name.append("13");
    }
    if (has_flag(extensions, ChordExtension::Eleventh)) {
      return name.append("11");
    }
    if (has_flag(extensions, ChordExtension::Ninth)) {
      return name.append("9");
    }
    if (has_flag(extensions, ChordExtension::Seventh)) {
      return name.append("7");
    }
    if (has_flag(extensions, ChordExtension::Sixth)) {
      return name.append("6");
    }
    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam N
  /// @param name
  /// @param alterations
  /// @return
  template<std::size_t N>
  static bool append_alteration_name(MString<N>& name,
                                     ChordAlteration alterations)
  {
    if (has_flag(alterations, ChordAlteration::Flat5)) {
      if (!name.append("b5")) {
        return false;
      }
    }
    if (has_flag(alterations, ChordAlteration::Sharp5)) {
      if (!name.append("#5")) {
        return false;
      }
    }
    if (has_flag(alterations, ChordAlteration::Flat9)) {
      if (!name.append("b9")) {
        return false;
      }
    }
    if (has_flag(alterations, ChordAlteration::Sharp9)) {
      if (!name.append("#9")) {
        return false;
      }
    }
    if (has_flag(alterations, ChordAlteration::Sharp11)) {
      if (!name.append("#11")) {
        return false;
      }
    }
    if (has_flag(alterations, ChordAlteration::Flat13)) {
      if (!name.append("b13")) {
        return false;
      }
    }
    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param scale
  /// @param root_index
  /// @param scale_span
  /// @param degrees_per_period
  /// @return
  static int get_scale_span_interval(const ScaleMap<SCALE_DEGREES>& scale,
                                     int root_index,
                                     int scale_span,
                                     int degrees_per_period)
  {
    int root_period_offset = 0;
    int target_period_offset = 0;
    const Degree root_degree =
      scale.get_mapped_degree(root_index, root_period_offset);
    const Degree target_degree =
      scale.get_mapped_degree(root_index + scale_span, target_period_offset);
    return (target_degree + (target_period_offset * degrees_per_period)) -
           (root_degree + (root_period_offset * degrees_per_period));
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param scale_index
  /// @return
  static const char* roman_numeral_for_scale_index(int scale_index)
  {
    switch (wrap(scale_index, SCALE_CHORD_COUNT)) {
      case 0:
        return "I";
      case 1:
        return "II";
      case 2:
        return "III";
      case 3:
        return "IV";
      case 4:
        return "V";
      case 5:
        return "VI";
      case 6:
        return "VII";
      default:
        return "?";
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param scale_index
  /// @return
  static const char* lowercase_roman_numeral_for_scale_index(int scale_index)
  {
    switch (wrap(scale_index, SCALE_CHORD_COUNT)) {
      case 0:
        return "i";
      case 1:
        return "ii";
      case 2:
        return "iii";
      case 3:
        return "iv";
      case 4:
        return "v";
      case 5:
        return "vi";
      case 6:
        return "vii";
      default:
        return "?";
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param actual
  /// @param expected
  /// @return
  static bool matches_interval_label(const char* actual, const char* expected)
  {
    if ((actual == nullptr) || (expected == nullptr)) {
      return false;
    }

    std::size_t i = 0;
    while (expected[i] != '\0') {
      if (actual[i] != expected[i]) {
        return false;
      }
      ++i;
    }
    return actual[i] == '\0';
  }
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
struct ChordTone
{
  ScaleDegree degree; // temeprament or scale degree
  Period preferred_period;
  uint8_t flags; // optional: chord root, avoid doubling, tendency tone, etc.
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
struct ChordProbability
{
  ScaleDegree from_degree;
  float to_degree[SCALE_CHORD_COUNT];
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
static const ScaleDegree MAJOR_SCALE_DEGREES[SCALE_CHORD_COUNT] = {
  ScaleDegree::I, ScaleDegree::ii, ScaleDegree::iii,  ScaleDegree::IV,
  ScaleDegree::V, ScaleDegree::vi, ScaleDegree::vii_o
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
static const ScaleDegree MINOR_SCALE_DEGREES[SCALE_CHORD_COUNT] = {
  ScaleDegree::i, ScaleDegree::ii_o, ScaleDegree::III, ScaleDegree::iv,
  ScaleDegree::v, ScaleDegree::VI,   ScaleDegree::VII
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
static const float MAJOR_STARTING_CHORD_WEIGHTS[SCALE_CHORD_COUNT] = {
  4.0F, 0.50F, 0.25F, 1.25F, 2.50F, 1.50F, 0.10F
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
static const float MINOR_STARTING_CHORD_WEIGHTS[SCALE_CHORD_COUNT] = {
  4.0F, 0.20F, 1.25F, 1.00F, 1.50F, 1.25F, 1.00F
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
static const ChordProbability MajorScaleChordProbability[SCALE_CHORD_COUNT] = {
  //           I      ii     iii    IV     V      vi     vii_0
  { ScaleDegree::I,
    /******/ { 0.10F, 0.25F, 0.25F, 1.00F, 1.00F, 0.50F, 0.05F } },
  { ScaleDegree::ii,
    /******/ { 0.25F, 0.10F, 0.25F, 0.50F, 1.00F, 0.50F, 0.05F } },
  { ScaleDegree::iii,
    /******/ { 0.25F, 0.25F, 0.10F, 0.50F, 0.25F, 0.75F, 0.05F } },
  { ScaleDegree::IV,
    /******/ { 0.50F, 0.50F, 0.25F, 0.10F, 1.00F, 0.25F, 0.05F } },
  { ScaleDegree::V,
    /******/ { 1.00F, 0.25F, 0.25F, 0.50F, 0.10F, 0.50F, 0.05F } },
  { ScaleDegree::vi,
    /******/ { 0.25F, 1.00F, 0.50F, 0.50F, 1.00F, 0.10F, 0.05F } },
  { ScaleDegree::vii_o,
    /******/ { 1.00F, 0.25F, 0.50F, 0.25F, 0.25F, 0.50F, 0.05F } },
};

static const ChordProbability MinorScaleChordProbability[SCALE_CHORD_COUNT] = {
  //            i      ii_o   III    iv     v      VI     VII
  { ScaleDegree::i,
    /*******/ { 0.10F, 0.15F, 0.75F, 1.00F, 1.00F, 0.75F, 0.75F } },
  { ScaleDegree::ii_o,
    /*******/ { 1.00F, 0.10F, 0.15F, 0.50F, 1.00F, 0.25F, 0.25F } },
  { ScaleDegree::III,
    /*******/ { 0.75F, 0.15F, 0.10F, 0.75F, 0.50F, 1.00F, 0.75F } },
  { ScaleDegree::iv,
    /*******/ { 0.75F, 0.25F, 0.50F, 0.10F, 1.00F, 0.75F, 0.50F } },
  { ScaleDegree::v,
    /*******/ { 1.00F, 0.25F, 0.25F, 0.75F, 0.10F, 0.50F, 0.75F } },
  { ScaleDegree::VI,
    /*******/ { 0.50F, 0.75F, 0.50F, 1.00F, 0.75F, 0.10F, 1.00F } },
  { ScaleDegree::VII,
    /*******/ { 1.00F, 0.15F, 0.50F, 0.75F, 0.25F, 0.75F, 0.10F } },
};

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param mode
/// @return
static inline const ScaleDegree*
scale_degrees_for_mode(HarmonicMode mode)
{
  return (mode == HarmonicMode::Minor) ? MINOR_SCALE_DEGREES
                                       : MAJOR_SCALE_DEGREES;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param mode
/// @return
static inline const ChordProbability*
chord_probability_table_for_mode(HarmonicMode mode)
{
  return (mode == HarmonicMode::Minor) ? MinorScaleChordProbability
                                       : MajorScaleChordProbability;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param mode
/// @return
static inline const float*
starting_chord_weights_for_mode(HarmonicMode mode)
{
  return (mode == HarmonicMode::Minor) ? MINOR_STARTING_CHORD_WEIGHTS
                                       : MAJOR_STARTING_CHORD_WEIGHTS;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Gets the index of a scale degree within the context of a harmonic
/// mode.
/// @param degree The scale degree to find the index of.
/// @param mode The harmonic mode.
/// @return int The index of the scale degree, or -1 if not found.
static inline int
scale_degree_index(ScaleDegree degree, HarmonicMode mode)
{
  const ScaleDegree* degrees = scale_degrees_for_mode(mode);
  for (std::size_t i = 0; i < SCALE_CHORD_COUNT; ++i) {
    if (degrees[i] == degree) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param index
/// @param mode
/// @return
static inline ScaleDegree
scale_degree_from_index(int index, HarmonicMode mode)
{
  index = std::max(index, 0);

  const int wrapped = index % static_cast<int>(SCALE_CHORD_COUNT);
  return scale_degrees_for_mode(mode)[wrapped];
}

///////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param degree
/// @return
static inline Note
scale_degree_to_semitone(ScaleDegree degree)
{
  switch (degree) {
    case ScaleDegree::I:
      return Note_P1;
    case ScaleDegree::ii:
      return Note_M2;
    case ScaleDegree::iii:
      return Note_M3;
    case ScaleDegree::IV:
      return Note_P4;
    case ScaleDegree::V:
      return Note_P5;
    case ScaleDegree::vi:
      return Note_M6;
    case ScaleDegree::vii_o:
      return Note_M7;

    case ScaleDegree::i:
      return Note_P1;
    case ScaleDegree::ii_o:
      return Note_M2;
    case ScaleDegree::III:
      return Note_m3;
    case ScaleDegree::iv:
      return Note_P4;
    case ScaleDegree::v:
      return Note_P5;
    case ScaleDegree::VI:
      return Note_m6;
    case ScaleDegree::VII:
      return Note_m7;
    default:
      return Note_P1;
  }
}

} // namespace Music
