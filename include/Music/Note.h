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

namespace Music {

struct NoteEvent {
  NoteEvent() {
    note = REST;
    period = 0;
    value = NoteValue::None;
  }

  NoteEvent(Note n, Period p, NoteValue v) {
    note = n;
    period = p;
    value = v;
  }

  Note note;
  Period period;
  NoteValue value;
};

}  // namespace Music
