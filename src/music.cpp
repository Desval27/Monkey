/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Music.cpp
 * @brief General Music Functions
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @brief
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#include <music/music.hpp>

namespace music {
// C  C#  D  D#  E   F  F#  G  G#   A   A#   B
// I  ii  II iii III IV TT  V  V+   vi  vii  VII
// 0  1   2  3   4   5  6   7  8    9   10   11

//////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param weights
/// @param count
/// @return
static int
weighted_index_from_row(const float* weights, std::size_t count)
{
  if ((weights == nullptr) || count == 0) {
    return 0;
  }

  float sum = 0.0F;
  for (std::size_t i = 0; i < count; ++i) {
    if (weights[i] > 0.0F) {
      sum += weights[i];
    }
  }

  if (sum <= 0.0F) {
    return 0;
  }

  const float unitRandom = randomRange(0.0F, 0.999999F);
  const float pick = unitRandom * sum;
  float accum = 0.0F;

  for (std::size_t i = 0; i < count; ++i) {
    const float w = (weights[i] > 0.0F) ? weights[i] : 0.0F;
    accum += w;
    if (pick < accum) {
      return static_cast<int>(i);
    }
  }

  return static_cast<int>(count - 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param low
/// @param high
/// @return
NoteValue
get_random_granularity(NoteValue low, NoteValue high)
{
  // This is really weird but it will be addressed later with some sort of
  // table.
  if (low == NoteValue::None) {
    return NoteValue::None;
  }

  int div = static_cast<int>(high) / static_cast<int>(low);
  int tries = 0;
  NoteValue v;
  do {
    int cnt = randomRange(1, div);
    v = static_cast<NoteValue>(low * cnt);
    tries++;
  } while (is_note_value_weird(v) && tries <= 100);

#ifdef DEBUG_COUT
  std::clog << "Got Random G = " << v << " tried " << tries << " times."
            << std::endl;
#endif

  return v;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
const char*
get_note_value_text(NoteValue v)
{
  // Crude but workable for now.
  switch (v) {
#if PPQN >= 32
    case NoteValue::SixtyFourth:
      return v_SixtyFourthNote;
    case NoteValue::DottedSixtyFourth:
      return v_DSixteenthNote;
    case NoteValue::DottedDottedThirtySecond:
      return v_DDThirtySecondNote;
#endif
    case NoteValue::ThirtySecond:
      return v_ThirtySecondNote;
    case NoteValue::DottedThirtySecond:
      return v_DThirtySecondNote;

    case NoteValue::Sixteenth:
      return v_SixteenthNote;
    case NoteValue::DottedSixteenth:
      return v_DSixteenthNote;
    case NoteValue::DottedDottedSixteenth:
      return v_DDSixteenthNote;

    case NoteValue::Eighth:
      return v_EighthNote;
    case NoteValue::DottedEighth:
      return v_DEighthNote;
    case NoteValue::DottedDottedEighth:
      return v_DDEighthNote;

    case NoteValue::Quarter:
      return v_QuarterNote;
    case NoteValue::DottedQuarter:
      return v_DQuarterNote;
    case NoteValue::DottedDottedQuarter:
      return v_DDQuarterNote;

    case NoteValue::Half:
      return v_HalfNote;
    case NoteValue::DottedHalf:
      return v_DHalfNote;
    case NoteValue::DottedDottedHalf:
      return v_DDHalfNote;

    case NoteValue::Whole:
      return v_WholeNote;
    case NoteValue::DottedWhole:
      return v_DWholeNote;
    case NoteValue::DottedDottedWhole:
      return v_DDWholeNote;
    default:
#ifdef DEBUG_COUT
      std::clog << " (Undefined = " << v << ") ";
#endif
      return v_Undefined;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param mode
/// @return
ScaleDegree
get_weighted_starting_chord(HarmonicMode mode)
{
  const int index = weighted_index_from_row(
    starting_chord_weights_for_mode(mode), SCALE_CHORD_COUNT);
  return scale_degree_from_index(index, mode);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief
/// @param fromDegree
/// @param mode
/// @return
ScaleDegree
get_weighted_next_chord(ScaleDegree fromDegree, HarmonicMode mode)
{
  const int fromIndex = scale_degree_index(fromDegree, mode);
  if (fromIndex < 0) {
    return get_weighted_starting_chord(mode);
  }

  const ChordProbability* table = chord_probability_table_for_mode(mode);
  const int toIdx =
    weighted_index_from_row(table[fromIndex].toDegree, SCALE_CHORD_COUNT);
  return scale_degree_from_index(toIdx, mode);
}

} // namespace Music
