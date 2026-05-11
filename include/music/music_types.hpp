/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file music_types.h
 * @brief Music data Type Definitions.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <array>
#include <cstdint>

#include <Music/music_config.hpp>
#include <mstring.hpp>

namespace music {

/// @brief The number of clock pulses per quarter note.
// This is independent of the beat unit of a time signature.
// Example: If a time signature's beat unit is an eighth note the
// number of pulses for it will still be PPQN / 2.
#define PPQN 24

///////////////////////////////////////////////////////////////////////////////
// Core Music Types
///////////////////////////////////////////////////////////////////////////////
using Note = int8_t;
using Degree = int8_t;
using Period = int8_t;

/**
 * @brief Represents a scale degree within the musical context.
 *
 */
enum class ScaleDegree : uint8_t
{
  // Major Scale Degrees
  I,     // Major First
  ii,    // Minor Second
  iii,   // Minor Third
  IV,    // Major Fourth
  V,     // Major Fifth
  vi,    // Minor Sixth
  vii_o, // Diminished Seventh

  // Minor Scale Degrees
  i,    // Minor First
  ii_o, // Diminished Second
  III,  // Major Third
  iv,   // Minor Fourth
  v,    // Minor Fifth
  VI,   // Major Sixth
  VII,  // Major Seventh
};

///////////////////////////////////////////////////////////////////////////////
enum class HarmonicMode : uint8_t
{
  Major,
  Minor,
};

///////////////////////////////////////////////////////////////////////////////
enum class Articulation : uint8_t
{
  None,
  Normal,
  Accent,
  Fermata,
  Legato,
  Marcato,
  MezzoStaccato,
  Sforzando,
  Staccato,
  Tenuto,
};

enum class Ornament : uint8_t
{
  None,
  Acciaccatura,
  Appoggiatura,
  Glissando,
  Mordent,
  Nachschlag,
  Slide,
  Trill,
  Turn,
};

/**
 * @brief Represents a type of musical scale based on the number of degrees it
 * contains. This enumeration provides a way to classify scales according to
 * their degree count, which can be used to identify common scale types such as
 * pentatonic, hexatonic, heptatonic, etc., as well as more unusual scales with
 * different numbers of degrees. The ScaleType can be used in conjunction with
 * scale definitions to facilitate the generation and analysis of musical
 * patterns and structures.
 */
enum ScaleType : uint8_t
{
  MONOTONIC = 1,
  DITONIC = 2,
  TRITONIC = 3,
  TETRATONIC = 4,
  PENTATONIC = 5,
  HEXATONIC = 6,
  HEPATONIC = 7,
  OCTATONIC = 8,
  NONATONIC = 9,
  DECATONIC = 11,
  UNDECATONIC = 11,
  DUODECATONIC = 12,
};

///////////////////////////////////////////////////////////////////////////////
// Convenient Type Aliases
///////////////////////////////////////////////////////////////////////////////
template<std::size_t N = HEPATONIC>
using DegreeMap = std::array<Degree, N>;
template<std::size_t N = HEPATONIC>
using WeightMap = std::array<float, N>;

///////////////////////////////////////////////////////////////////////////////
// Core Music Constants
///////////////////////////////////////////////////////////////////////////////
constexpr float BASE_HZ = 440.0F; // A4 = 440Hz
constexpr float FREQ_C0 = 16.35F;
constexpr float C4FREQ = 261.6256F;
constexpr Period MIN_PERIOD = -1;
constexpr Period MAX_PERIOD = 10;
constexpr int TWELVE_TONE = 12;
constexpr float OCTAVE_DOUBLE = 2.0F;
constexpr float ALMOST_ONE = 0.999999F;

// for convience for now.
constexpr Note REST = -1;
constexpr Note Note_P1 = 0;
constexpr Note Note_m2 = 1;
constexpr Note Note_M2 = 2;
constexpr Note Note_m3 = 3;
constexpr Note Note_M3 = 4;
constexpr Note Note_P4 = 5;
constexpr Note Note_TT = 6;
constexpr Note Note_P5 = 7;
constexpr Note Note_m6 = 8;
constexpr Note Note_M6 = 9;
constexpr Note Note_m7 = 10;
constexpr Note Note_M7 = 11;

///////////////////////////////////////////////////////////////////////////////
// Common Music data Structures
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Represents a reference pitch within the musical context, defined by a
 * base frequency in Hertz (Hz), a reference scale degree, and a reference
 * period. This structure serves as a fundamental reference point for
 * calculating the frequencies of other pitches in the tuning system based on
 * their relationships to this reference pitch in terms of scale degrees and
 * periods. The base frequency provides the actual pitch in terms of sound,
 * while the reference degree and period allow for understanding the pitch's
 * position within the musical scale and its relationship to other pitches in
 * terms of intervals and octave equivalence.
 */
struct PitchRef
{
  float baseHz = BASE_HZ; ///< The reference frequency in Hertz (Hz) for the
                          ///< tuning system, typically representing a standard
                          ///< pitch such as A4 = 440Hz.
  Degree baseDegree =
    0; ///< The reference scale degree associated with the reference
       ///< frequency, indicating its position within the musical scale.
       ///< This value can be used to determine the pitch's relationship to
       ///< other pitches in terms of intervals and harmonic function.
  Period basePeriod =
    0; ///< The reference period associated with the reference frequency,
       ///< indicating its position within the periodic structure of the
       ///< tuning system. This value can be used to determine the pitch's
       ///< relationship to other pitches in terms of octave equivalence
       ///< and overall pitch height.
};

/**
 * @brief Represents a tempered pitch within the musical context, defined by
 * its scale degree, period, and fine-tuning adjustment in cents. This
 * structure allows for precise representation of pitches in various tuning
 * systems, including equal temperament and microtonal tunings, by providing
 * the necessary information to calculate the actual frequency of the pitch
 * based on its relationship to a reference pitch and its position within the
 * musical scale and periodic structure.
 */
struct TemperedPitch
{
  Degree degree; ///< The scale degree of the pitch, indicating its position
                 ///< within the musical scale. This value can be used to
                 ///< determine the pitch's relationship to other pitches in
                 ///< terms of intervals and harmonic function.
  Period
    period; ///< The period of the pitch, indicating its position within the
            ///< periodic structure of the tuning system. This value can be
            ///< used to determine the pitch's relationship to other pitches
            ///< in terms of octave equivalence and overall pitch height.
  float fine_cents; ///< The fine-tuning adjustment in cents, allowing for
                    ///< precise tuning of the pitch beyond the standard scale
                    ///< degrees and periods. This value can be used to achieve
                    ///< microtonal adjustments or to fine-tune pitches for
                    ///< specific musical contexts.

  /////////////////////////////////////////////////////////////////////////////
  TemperedPitch()
    : degree(0)
    , period(0)
    , fine_cents(0.0F)
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param d
  /// @param p
  /// @param fc
  TemperedPitch(Degree d, Period p, float fc = 0.0F)
    : degree(d)
    , period(p)
    , fine_cents(fc)
  {
  }
};

/**
 * @brief Represents a tuning reference within the musical context.
 */
struct TuningReference
{
  float reference_hz; ///< The reference frequency in Hertz (Hz) for the tuning
                      ///< system.
  Degree reference_degree; ///< The reference scale degree associated with the
                           ///< reference frequency, indicating its position
                           ///< within the musical scale.
  Period
    reference_period; ///< The reference period associated with the reference
                      ///< frequency, indicating its position within the
                      ///< periodic structure of the tuning system.

  /**
   * @brief Construct a new Tuning Reference object
   *
   */
  TuningReference()
    : reference_hz(BASE_HZ)
    , reference_degree(0)
    , reference_period(0)
  {
  }

  /**
   * @brief Construct a new Tuning Reference object with specific values
   *
   * @param hz The reference frequency in Hertz (Hz) for the tuning system.
   * @param d The reference scale degree associated with the reference
   * frequency, indicating its position within the musical scale.
   * @param p The reference period associated with the reference frequency,
   * indicating its position within the periodic structure of the tuning
   * system.
   */
  TuningReference(float hz, Degree d, Period p)
    : reference_hz(hz)
    , reference_degree(d)
    , reference_period(p)
  {
  }
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Attack/Decay Envelope Parameters
struct ADEnvelope
{
  ADEnvelope()
  {
    attack = 0.0F;
    decay = 0.0F;
  }
  float attack; ///< In Seconds
  float decay;  ///< In Seconds
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Attack/Release Envelope Parameters
struct AREnvelope
{
  AREnvelope()
  {
    attack = 0.0F;
    release = 0.0F;
  }
  float attack;  ///< In Seconds
  float release; ///< In Seconds
};

////////////////////////////////////////////////////////////////////////////////
/// @brief Attack/Decay/Sustain/Release Parameters
struct ADSREnvelope
{
  ADSREnvelope()
  {
    attack = 0.0F;
    decay = 0.0F;
    sustain = 0.0F;
    release = 0.0F;
  }
  float attack;  ///< In Seconds
  float decay;   ///< In Seconds
  float sustain; ///< As 0.0 -> 1.0
  float release; ///< In Seconds
};

////////////////////////////////////////////////////////////////////////////////
struct EqualTemperamentTable
{
  int degrees_in_period;
  const MString<6> name;
  const char* const* note_labels;
  const char* const* interval_labels;
};

////////////////////////////////////////////////////////////////////////////////
template<std::size_t DEGREES, std::size_t SCALE_TYPE>
struct ScaleTable
{
  const char* name;
  HarmonicMode harmonic_mode;
  const DegreeMap<SCALE_TYPE>& degrees;

  [[nodiscard]] bool is_minor() const
  {
    return harmonic_mode == HarmonicMode::Minor;
  }
};

////////////////////////////////////////////////////////////////////////////////
template<std::size_t N>
struct WeightTable
{
  const char*
    name; ///< The name of the weight table, providing a human-readable
          ///< identifier for the set of weights it contains.
  const WeightMap<N>
    weights; ///< An array of weights corresponding to each degree in the
             ///< scale, where each weight represents the relative importance
             ///< or emphasis of that degree within the musical context. The
             ///< weights can be used for various purposes, such as
             ///< determining the likelihood of selecting a particular degree
             ///< when generating melodies or harmonies, or for analyzing the
             ///< structure of a musical piece based on the distribution of
             ///< emphasis across its degrees.
};

////////////////////////////////////////////////////////////////////////////////
struct NoteEventScore
{
  unsigned int overall =
    0; ///< Overall score representing the general quality or suitability
       ///< of a note event within a musical context, potentially taking
       ///< into account various factors such as harmonic compatibility,
       ///< rhythmic placement, and melodic contour.
  int density =
    0; ///< Score representing the density of a note event, which could be
       ///< based on factors such as the number of simultaneous notes
       ///< (chord density) or the frequency of note occurrences within a
       ///< given time frame (rhythmic density).
  int rests =
    0; ///< Score representing the presence and quality of rests in a
       ///< musical sequence, which could be based on factors such as the
       ///< duration and placement of rests, as well as their contribution
       ///< to the overall phrasing and musical expression.
  int cadence = 0; ///< Score representing the quality of cadences in a musical
                   ///< sequence, which could be based on factors such as the
                   ///< harmonic progression, rhythmic placement, and resolution
                   ///< of cadences, as well as their contribution to the
                   ///< overall structure and emotional impact of the music.
  int repetition =
    0; ///< Score representing the presence and quality of repetition in a
       ///< musical sequence, which could be based on factors such as the
       ///< frequency and placement of repeated motifs or phrases, as well
       ///< as their contribution to the overall coherence and memorability
       ///< of the music.
  int contour =
    0; ///< Score representing the melodic contour of a musical sequence,
       ///< which could be based on factors such as the direction and shape
       ///< of melodic lines, as well as their contribution to the overall
       ///< expressiveness and emotional impact of the music.
  int phrase = 0; ///< Score representing the quality of phrasing in a musical
                  ///< sequence, which could be based on factors such as the
                  ///< grouping and articulation of notes into musical phrases,
                  ///< as well as their contribution to the overall flow and
                  ///< expressiveness of the music.
  int rhythm =
    0; ///< Score representing the quality of rhythm in a musical sequence,
       ///< which could be based on factors such as the complexity and
       ///< variety of rhythmic patterns, as well as their contribution to
       ///< the overall energy and drive of the music.
};

} // namespace Music
