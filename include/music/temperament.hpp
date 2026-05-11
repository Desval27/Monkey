/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file temperament.hpp
 * @brief Musical Temperaments.
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * Embedded-friendly temperament / scale / pitch support
 * Suitable for Daisy, Arduino, STM32, etc.
 * No dynamic allocation.
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#include <Music/music_helpers.hpp>
#include <Music/music_types.hpp>
#include <monkey.hpp>

namespace music {

/**
 * @brief A set of string labels for degrees, notes, or intervals in a
 * temperament.
 *
 */

template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES>
class LabelSet
{
public:
  LabelSet() = default;

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param labels
  /// @param count
  /// @return
  bool attach(const char* const* labels, uint16_t count)
  {
    if ((labels == nullptr) || count == 0 || count > MAX_DEGREES) {
      return false;
    }

    labels_ = labels;
    count_ = count;
    return true;
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @tparam N
  /// @param index
  /// @param out
  /// @return
  template<std::size_t N>
  bool get(Degree index, MString<N>& out) const
  {
    if (!labels_ || index < 0 || index >= count_) {
      return false;
    }

    const char* src = TUNING_PGM_READ_PTR(&labels_[index]);
    return out.set(src);
  }

  /////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param index
  /// @param out
  /// @param max_out
  /// @return
  bool get(Degree index, char* out, std::size_t max_out) const
  {
    if ((labels_ == nullptr) || index >= count_ || (out == nullptr) ||
        max_out == 0) {
      return false;
    }

    const char* src = TUNING_PGM_READ_PTR(&labels_[index]);
    tuning_copy_string(out, src, max_out);
    return true;
  }

private:
  uint16_t count_{ 0 };
  const char* const* labels_{ nullptr };
};

/**
 * @brief Represents a musical temperament system with ratios and labels for
 * pitches.
 *
 */
template<std::size_t MAX_DEGREES = DEF_MAX_DEGREES,
         int8_t MIN_PERIOD = -4,
         int8_t MAX_PERIOD = 4>
class Temperament
{
public:
  enum class Kind : uint8_t
  {
    RatioTable = 0,
    EqualDivision
  };

  /**
   * @brief Construct a new Temperament object with equal division of the
   * octave.
   *
   * @param degrees_per_period Number of degrees per period (e.g., 12 for
   * 12-EDO).
   * @param period_ratio The ratio of the period (e.g., 2.0 for octave).
   */
  Temperament(uint16_t degrees_per_period = 12, float period_ratio = 2.0F)
  {
    clear();
    make_equal_division(degrees_per_period, period_ratio);
  }

  /**
   * @brief reset the temperament to default state with no ratios.
   *
   */
  void clear()
  {
    kind_ = Kind::RatioTable;
    degrees_per_period_ = 0;
    period_ratio_ = OCTAVE_DOUBLE;

    // Defaults to no ratios
    for (std::size_t i = 0; i < MAX_DEGREES; ++i) {
      ratios_[i] = 1.0F;
    }

    rebuild_period_multipliers();
  }

  /**
   * @brief Create an equal division temperament (e.g., 12-EDO).
   *
   * @param degrees_per_period Number of equal steps per period.
   * @param period_ratio The ratio of the period (e.g., 2.0 for octave).
   * @return true if successful, false if parameters are invalid.
   * @return false
   */
  bool make_equal_division(uint16_t degrees_per_period, float period_ratio)
  {
    if (degrees_per_period == 0 || degrees_per_period > MAX_DEGREES ||
        period_ratio <= 0.0F) {
      return false;
    }

    kind_ = Kind::EqualDivision;
    degrees_per_period_ = degrees_per_period;
    period_ratio_ = period_ratio;

    for (uint16_t i = 0; i < degrees_per_period_; ++i) {
      ratios_[i] =
        powf(period_ratio_,
             static_cast<float>(i) / static_cast<float>(degrees_per_period_));
    }

    rebuild_period_multipliers();
    return true;
  }

  /**
   * @brief Create a temperament from a table of ratios.
   *
   * @param ratios Array of frequency ratios for each degree.
   * @param count Number of ratios in the array.
   * @param period_ratio The ratio of the period (e.g., 2.0 for octave).
   * @return true if successful, false if parameters are invalid.
   * @return false
   */
  template<typename TEventType, std::size_t N>
  bool make_ratio_table(const std::array<TEventType, N>& ratios,
                        float period_ratio = 2.0F)
  {
    static_assert(std::is_floating_point<TEventType>::value,
                  "T must be a floating-point type");

    if (ratios.empty() || ratios.size() > MAX_DEGREES || period_ratio <= 0.0F) {
      return false;
    }

    kind_ = Kind::RatioTable;
    degrees_per_period_ = ratios.size();
    period_ratio_ = period_ratio;

    for (uint16_t i = 0; i < ratios.size(); ++i) {
      ratios_[i] = ratios[i];
    }

    rebuild_period_multipliers();
    return true;
  }

  /**
   * @brief Create a temperament from normalized ratios, automatically sorting
   * and normalizing.
   *
   * @param ratios Array of frequency ratios to be normalized.
   * @param count Number of ratios in the array.
   * @param period_ratio The ratio of the period (e.g., 2.0 for octave).
   * @return true if successful, false if parameters are invalid.
   * @return false
   */
  template<typename TEventType, std::size_t N>
  bool make_normalized_ratio_table(const std::array<TEventType, N>& ratios,
                                   float period_ratio = 2.0F)
  {
    if (ratios.empty() || ratios.size() > MAX_DEGREES || period_ratio <= 1.0F) {
      return false;
    }

    kind_ = Kind::RatioTable;
    degrees_per_period_ = ratios.size();
    period_ratio_ = period_ratio;

    for (uint16_t i = 0; i < ratios.size(); ++i) {
      float r = ratios[i];
      if (r <= 0.0F) {
        return false;
      }

      while (r < 1.0F) {
        r *= period_ratio_;
      }
      while (r >= period_ratio_) {
        r /= period_ratio_;
      }

      ratios_[i] = r;
    }

    sort_ratios();
    rebuild_period_multipliers();
    return true;
  }

  /**
   * @brief attach labels for note names (e.g., "C", "C#", "D").
   *
   * @param labels Array of string pointers for note labels.
   * @param count Number of labels.
   * @return true if successful, false if parameters are invalid.
   * @return false
   */
  bool attach_note_labels(const char* const* labels, uint16_t count)
  {
    return note_labels_.attach(labels, count);
  }

  template<typename TEventType, std::size_t N>
  bool attach_note_labels(const std::array<TEventType, N>& labels)
  {
    return note_labels_.attach(labels.data(), labels.size());
  }

  /**
   * @brief attach labels for interval names (e.g., "unison", "major third").
   *
   * @param labels Array of string pointers for interval labels.
   * @param count Number of labels.
   * @return true if successful, false if parameters are invalid.
   * @return false
   */
  bool attach_interval_labels(const char* const* labels, uint16_t count)
  {
    return interval_labels_.attach(labels, count);
  }

  template<typename TEventType, std::size_t N>
  bool attach_interval_labels(const std::array<TEventType, N>& labels)
  {
    return interval_labels_.attach(labels.data(), labels.size());
  }

  /**
   * @brief get the string label for a given degree (note).
   *
   * @param degree The degree to get the label for.
   * @param out Buffer to write the label to.
   * @param max_out Maximum size of the output buffer.
   * @return true if successful, false if degree is invalid or no labels
   * attached.
   * @return false
   */
  bool get_note_label(Degree degree, char* out, std::size_t max_out) const
  {
    if (degrees_per_period_ == 0) {
      return false;
    }

    if (degree == REST) {
      return strncpy(out, "REST", max_out) != nullptr;
    }

    return note_labels_.get(
      static_cast<Degree>(
        wrap(static_cast<int>(degree), static_cast<int>(degrees_per_period_))),
      out,
      max_out);
  }

  /**
   * @brief get the string label for a given degree (note).
   *
   * @param degree The degree to get the label for.
   * @param out Buffer to write the label to.
   * @return true if successful, false if degree is invalid or no labels
   * attached.
   * @return false
   */
  template<std::size_t N>
  bool get_note_label(Degree degree, MString<N>& out) const
  {
    if (degrees_per_period_ == 0) {
      return false;
    }

    if (degree == REST) {
      return out.set("REST");
    }

    return note_labels_.get(
      static_cast<Degree>(
        wrap(static_cast<int>(degree), static_cast<int>(degrees_per_period_))),
      out);
  }

  /**
   * @brief Get the string label for a given interval degree.
   *
   * @param degree The interval degree to get the label for.
   * @param out Buffer to write the label to.
   * @param max_out Maximum size of the output buffer.
   * @return true if successful, false if degree is invalid or no labels
   * attached.
   * @return false
   */
  bool get_interval_label(Degree degree, char* out, std::size_t max_out) const
  {
    if (degrees_per_period_ == 0) {
      return false;
    }

    return interval_labels_.get(
      static_cast<Degree>(
        wrap(static_cast<int>(degree), static_cast<int>(degrees_per_period_))),
      out,
      max_out);
  }

  /**
   * @brief Get the string label for a given interval degree.
   *
   * @param degree The interval degree to get the label for.
   * @param out Buffer to write the label to.
   * @return true if successful, false if degree is invalid or no labels
   * attached.
   * @return false
   */
  template<std::size_t N>
  bool get_interval_label(Degree degree, MString<N>& out) const
  {
    if (degrees_per_period_ == 0) {
      return false;
    }

    return interval_labels_.get(
      static_cast<Degree>(
        wrap(static_cast<int>(degree), static_cast<int>(degrees_per_period_))),
      out);
  }

  /**
   * @brief Get the number of degrees per period in this temperament.
   *
   * @return uint16_t Number of degrees per period.
   */
  [[nodiscard]] uint16_t degrees_per_period() const
  {
    return degrees_per_period_;
  }

  /**
   * @brief Get the ratio of the period (e.g., 2.0 for octave).
   *
   * @return float The period ratio.
   */
  [[nodiscard]] float period_ratio() const { return period_ratio_; }

  /**
   * @brief Get the frequency ratio for a given degree within one period.
   *
   * @param degree The degree to get the ratio for.
   * @return float The frequency ratio (1.0 for unison).
   */
  [[nodiscard]] float ratio_at(Degree degree) const
  {
    if (degrees_per_period_ == 0) {
      return 1.0F;
    }

    int idx =
      wrap(static_cast<int>(degree), static_cast<int>(degrees_per_period_));
    return ratios_[idx];
  }

  /**
   * @brief Get the multiplier for a given period offset.
   *
   * @param period The period offset (e.g., 1 for one octave up).
   * @return float The period multiplier.
   */
  [[nodiscard]] float period_multiplier(Period period) const
  {
    if (period >= MIN_PERIOD && period <= MAX_PERIOD) {
      return period_multipliers_[period - MIN_PERIOD];
    }

    return fastPow(period_ratio_, period);
  }

  /**
   * @brief Get the combined ratio for a degree and period offset.
   *
   * @param degree The degree within the period.
   * @param period The period offset.
   * @return float The combined frequency ratio.
   */
  [[nodiscard]] float ratio_at(int degree, Period period) const
  {
    return ratio_at(degree) * period_multiplier(period);
  }

  /**
   * @brief Calculate frequency from a root frequency, degree, and period.
   *
   * @param root_hz The root frequency in Hz.
   * @param degree The degree within the period.
   * @param period The period offset.
   * @param fine_cents Fine tuning adjustment in cents.
   * @return float The calculated frequency in Hz.
   */
  [[nodiscard]] float frequency_from_root(float root_hz,
                                          Degree degree,
                                          Period period = 0,
                                          float fine_cents = 0.0F) const
  {
    float ratio = ratio_at(degree, period);
    if (fine_cents != 0.0F) {
      ratio *= CentsToRatio(fine_cents);
    }
    return root_hz * ratio;
  }

  /**
   * @brief Calculate frequency from C0 (16.35 Hz), degree, and period.
   *
   * @param degree The degree within the period.
   * @param period The period offset.
   * @param fine_cents Fine tuning adjustment in cents.
   * @return float The calculated frequency in Hz.
   */
  [[nodiscard]] float frequency_from_C0(Degree degree,
                                        Period period = 0,
                                        float fine_cents = 0.0F) const
  {
    return frequency_from_root(FREQ_C0, degree, period, fine_cents);
  }

  /**
   * @brief Calculate frequency from a tuning reference pitch.
   *
   * @param pitch The tempered pitch to calculate.
   * @param ref The tuning reference (root frequency and degree).
   * @return float The calculated frequency in Hz.
   */
  [[nodiscard]] float frequency_from_reference(const TemperedPitch& pitch,
                                               const TuningReference& ref) const
  {
    int relDegree = pitch.degree - ref.reference_degree;
    int relPeriod = pitch.period - ref.reference_period;

    float ratio = ratio_at(relDegree, relPeriod);

    if (pitch.fine_cents != 0.0F) {
      ratio *= CentsToRatio(pitch.fine_cents);
    }

    return ref.reference_hz * ratio;
  }

  /**
   * @brief Find the nearest degree for a given frequency ratio.
   *
   * @param target_ratio The target frequency ratio.
   * @param out_degree Output parameter for the nearest degree.
   * @param out_error_cents Output parameter for the error in cents.
   * @return true if successful, false if no valid degree found.
   * @return false
   */
  bool find_nearest_degree_for_ratio(float target_ratio,
                                     Degree& out_degree,
                                     float& out_error_cents) const
  {
    if (degrees_per_period_ == 0 || target_ratio <= 0.0F) {
      return false;
    }

    while (target_ratio < 1.0F) {
      target_ratio *= period_ratio_;
    }
    while (target_ratio >= period_ratio_) {
      target_ratio /= period_ratio_;
    }

    Degree best = 0;
    float best_err = 1e30F;

    for (uint16_t i = 0; i < degrees_per_period_; ++i) {
      float err = RatioToCents(target_ratio / ratios_[i]);
      float abs_err = fabsf(err);
      if (abs_err < best_err) {
        best_err = abs_err;
        best = static_cast<Degree>(i);
        out_error_cents = err;
      }
    }

    out_degree = best;
    return true;
  }

  /**
   * @brief Calculate the minimum distance between two notes in degrees.
   *
   * @param a First note.
   * @param b Second note.
   * @return int The distance in degrees (always positive).
   */
  [[nodiscard]] int get_note_distance(Note a, Note b) const
  {
    return abs(b - a) % degrees_per_period_;
  }

  /**
   * @brief Check if an interval is considered consonant in this temperament.
   *
   * TODO: This does not support anything but 12EDO
   * @param interval The interval degree to check.
   * @return true if consonant, false otherwise.
   */
  [[nodiscard]] bool is_consonant(Degree interval) const
  {
    switch (interval % degrees_per_period_) {
      case 0:
      case 3:
      case 4:
      case 5:
      case 7:
      case 8:
      case 9:
        return true;
      default:
        return false;
    }
  }

private:
  Kind kind_;
  uint16_t degrees_per_period_;
  float period_ratio_;
  float ratios_[MAX_DEGREES];
  float period_multipliers_[(MAX_PERIOD - MIN_PERIOD) + 1];
  LabelSet<MAX_DEGREES> note_labels_;
  LabelSet<MAX_DEGREES> interval_labels_;

  /**
   * @brief Rebuild the cached period multipliers array.
   *
   */
  void rebuild_period_multipliers()
  {
    for (Period p = MIN_PERIOD; p <= MAX_PERIOD; ++p) {
      period_multipliers_[p - MIN_PERIOD] = fastPow(period_ratio_, p);
    }
  }

  /**
   * @brief Sort the ratios array in ascending order using insertion sort.
   *
   */
  void sort_ratios()
  {
    for (uint16_t i = 1; i < degrees_per_period_; ++i) {
      float key = ratios_[i];
      int j = static_cast<int>(i) - 1;

      while (j >= 0 && ratios_[j] > key) {
        ratios_[j + 1] = ratios_[j];
        --j;
      }

      ratios_[j + 1] = key;
    }
  }
};

} // namespace Music
