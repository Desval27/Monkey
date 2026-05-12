/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file note.hpp
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <music/music_types.hpp>
#include <music/note_value.hpp>

namespace music {

/**
 * @brief Represents a single note event with pitch and value.
 * Note events are used for both active pitched notes and rests, which are
 * indicated by the special REST note value.
 */
struct NoteEvent
{
  static NoteEvent EmptyNote;

  NoteEvent()
  {
    note = REST;
    period = 0;
    value = NoteValue::None;
  }

  NoteEvent(Note n, Period p, NoteValue v)
  {
    note = n;
    period = p;
    value = v;
  }

  NoteEvent(Note n)
  {
    note = n;
    period = 0;
    value = NoteValue::None;
  }

  Note note; ///< The note inverval within a period, where 0 is the tonic note.
             ///< REST indicates a rest event with no pitch.
  Period period;   ///< Period offset from the root note, used for scale mapping
                   ///< and temperament frequency calculations.
  NoteValue value; ///< Duration of the note event, or NoteValue::None for an
                   ///< event with no value.

  /**
   * @brief
   */
  [[nodiscard]] bool IsPitched() const { return note != REST; }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam TEMPERAMENT_DEGREES
  /// @tparam N
  /// @param temperament
  /// @param out
  /// @return
  template<std::size_t TEMPERAMENT_DEGREES, std::size_t N>
  bool get_interval_name(const Temperament<TEMPERAMENT_DEGREES>& temperament,
                         MString<N>& out) const
  {
    return temperament.get_interval_label(note, out);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam TEMPERAMENT_DEGREES
  /// @tparam N
  /// @param temperament
  /// @param out
  /// @return
  template<std::size_t TEMPERAMENT_DEGREES, std::size_t N>
  bool get_note_name(const Temperament<TEMPERAMENT_DEGREES>& temperament,
                     MString<N>& out) const
  {
    return temperament.get_note_label(note, out);
  }
};

} // namespace Music
