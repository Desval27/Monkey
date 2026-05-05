/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Note.h
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/MusicTypes.h>
#include <Music/MusicConst.h>
#include <Music/NoteValue.h>

namespace Music
{

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

    Note note;        ///< The note inverval within a period, where 0 is the tonic note. REST indicates a rest event with no pitch.
    Period period;    ///< Period offset from the root note, used for scale mapping and temperament frequency calculations.
    NoteValue value;  ///< Duration of the note event, or NoteValue::None for an event with no value.

    /**
     * @brief
     */
    bool IsPitched() const
    {
      return note != REST;
    }
  };

} // namespace Music
