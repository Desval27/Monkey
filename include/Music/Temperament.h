/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Temperament.h
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

#include <Monkey.h>
#include <Music/MusicConst.h>
#include <Music/MusicHelpers.h>
#include <Music/MusicTypes.h>

namespace Music
{

  /**
   * @brief A set of string labels for degrees, notes, or intervals in a temperament.
   *
   */
  template <std::size_t MAX_DEGREES = 64>
  class LabelSet
  {
  public:
    LabelSet() : _count(0), _labels(nullptr) {}

    bool Attach(const char *const *labels, uint16_t count)
    {
      if (!labels || count == 0 || count > MAX_DEGREES)
        return false;

      _labels = labels;
      _count = count;
      return true;
    }

    template <std::size_t N>
    bool Get(Degree index, MString<N> &out) const
    {
      if (!_labels || index < 0 || index >= _count)
        return false;

      const char *src = TUNING_PGM_READ_PTR(&_labels[index]);
      return out.set(src);
    }

    bool Get(Degree index, char *out, std::size_t maxOut) const
    {
      if (!_labels || index >= _count || !out || maxOut == 0)
        return false;

      const char *src = TUNING_PGM_READ_PTR(&_labels[index]);
      tuning_copy_string(out, src, maxOut);
      return true;
    }

  private:
    uint16_t _count;
    const char *const *_labels;
  };

  /**
   * @brief Represents a musical temperament system with ratios and labels for pitches.
   *
   */
  template <std::size_t MAX_DEGREES = DEF_MAX_DEGREES, int MIN_PERIOD = -4, int MAX_PERIOD = 4>
  class Temperament
  {
  public:
    enum class Kind : uint8_t
    {
      RatioTable = 0,
      EqualDivision
    };

    /**
     * @brief Construct a new Temperament object with equal division of the octave.
     *
     * @param degreesPerPeriod Number of degrees per period (e.g., 12 for 12-EDO).
     * @param periodRatio The ratio of the period (e.g., 2.0 for octave).
     */
    Temperament(uint16_t degreesPerPeriod = 12, float periodRatio = 2.0f)
    {
      Clear();
      MakeEqualDivision(degreesPerPeriod, periodRatio);
    }

    /**
     * @brief Reset the temperament to default state with no ratios.
     *
     */
    void Clear()
    {
      kind_ = Kind::RatioTable;
      degreesPerPeriod_ = 0;
      periodRatio_ = 2.0f;

      // Defaults to no ratios
      for (std::size_t i = 0; i < MAX_DEGREES; ++i)
        ratios_[i] = 1.0f;

      RebuildPeriodMultipliers();
    }

    /**
     * @brief Create an equal division temperament (e.g., 12-EDO).
     *
     * @param degreesPerPeriod Number of equal steps per period.
     * @param periodRatio The ratio of the period (e.g., 2.0 for octave).
     * @return true if successful, false if parameters are invalid.
     * @return false
     */
    bool MakeEqualDivision(uint16_t degreesPerPeriod, float periodRatio)
    {
      if (degreesPerPeriod == 0 || degreesPerPeriod > MAX_DEGREES ||
          periodRatio <= 0.0f)
        return false;

      kind_ = Kind::EqualDivision;
      degreesPerPeriod_ = degreesPerPeriod;
      periodRatio_ = periodRatio;

      for (uint16_t i = 0; i < degreesPerPeriod_; ++i)
        ratios_[i] = powf(periodRatio_, (float)i / (float)degreesPerPeriod_);

      RebuildPeriodMultipliers();
      return true;
    }

    /**
     * @brief Create a temperament from a table of ratios.
     *
     * @param ratios Array of frequency ratios for each degree.
     * @param count Number of ratios in the array.
     * @param periodRatio The ratio of the period (e.g., 2.0 for octave).
     * @return true if successful, false if parameters are invalid.
     * @return false
     */
    template <typename T, std::size_t N>
    bool MakeRatioTable(const std::array<T, N> &ratios, float periodRatio = 2.0f)
    {
      static_assert(std::is_floating_point<T>::value, "T must be a floating-point type");

      if (ratios.size() == 0 || ratios.size() > MAX_DEGREES || periodRatio <= 0.0f)
        return false;

      kind_ = Kind::RatioTable;
      degreesPerPeriod_ = ratios.size();
      periodRatio_ = periodRatio;

      for (uint16_t i = 0; i < ratios.size(); ++i)
        ratios_[i] = ratios[i];

      RebuildPeriodMultipliers();
      return true;
    }

    /**
     * @brief Create a temperament from normalized ratios, automatically sorting and normalizing.
     *
     * @param ratios Array of frequency ratios to be normalized.
     * @param count Number of ratios in the array.
     * @param periodRatio The ratio of the period (e.g., 2.0 for octave).
     * @return true if successful, false if parameters are invalid.
     * @return false
     */
    template <typename T, std::size_t N>
    bool MakeNormalizedRatioTable(const std::array<T, N> &ratios, float periodRatio = 2.0f)
    {
      if (ratios.size() == 0 || ratios.size() > MAX_DEGREES || periodRatio <= 1.0f)
        return false;

      kind_ = Kind::RatioTable;
      degreesPerPeriod_ = ratios.size();
      periodRatio_ = periodRatio;

      for (uint16_t i = 0; i < ratios.size(); ++i)
      {
        float r = ratios[i];
        if (r <= 0.0f)
          return false;

        while (r < 1.0f)
          r *= periodRatio_;
        while (r >= periodRatio_)
          r /= periodRatio_;

        ratios_[i] = r;
      }

      SortRatios();
      RebuildPeriodMultipliers();
      return true;
    }

    /**
     * @brief Attach labels for note names (e.g., "C", "C#", "D").
     *
     * @param labels Array of string pointers for note labels.
     * @param count Number of labels.
     * @return true if successful, false if parameters are invalid.
     * @return false
     */
    bool AttachNoteLabels(const char *const *labels, uint16_t count)
    {
      return noteLabels_.Attach(labels, count);
    }

    template <typename T, std::size_t N>
    bool AttachNoteLabels(const std::array<T, N> &labels)
    {
      return noteLabels_.Attach(labels.data(), labels.size());
    }

    /**
     * @brief Attach labels for interval names (e.g., "unison", "major third").
     *
     * @param labels Array of string pointers for interval labels.
     * @param count Number of labels.
     * @return true if successful, false if parameters are invalid.
     * @return false
     */
    bool AttachIntervalLabels(const char *const *labels, uint16_t count)
    {
      return intervalLabels_.Attach(labels, count);
    }

    template <typename T, std::size_t N>
    bool AttachIntervalLabels(const std::array<T, N> &labels)
    {
      return intervalLabels_.Attach(labels.data(), labels.size());
    }

    /**
     * @brief Get the string label for a given degree (note).
     *
     * @param degree The degree to get the label for.
     * @param out Buffer to write the label to.
     * @param maxOut Maximum size of the output buffer.
     * @return true if successful, false if degree is invalid or no labels attached.
     * @return false
     */
    bool GetNoteLabel(Degree degree, char *out, std::size_t maxOut) const
    {
      if (degreesPerPeriod_ == 0)
        return false;

      if (degree == REST)
        return strncpy(out, "REST", maxOut);
      else
        return noteLabels_.Get(
            static_cast<Degree>(wrap(static_cast<int>(degree),
                                     static_cast<int>(degreesPerPeriod_))),
            out, maxOut);
    }

    /**
     * @brief Get the string label for a given degree (note).
     *
     * @param degree The degree to get the label for.
     * @param out Buffer to write the label to.
     * @return true if successful, false if degree is invalid or no labels attached.
     * @return false
     */
    template <std::size_t N>
    bool GetNoteLabel(Degree degree, MString<N> &out) const
    {
      if (degreesPerPeriod_ == 0)
        return false;

      if (degree == REST)
        return out.set("REST");
      else
        return noteLabels_.Get(
            static_cast<Degree>(wrap(static_cast<int>(degree),
                                     static_cast<int>(degreesPerPeriod_))),
            out);
    }

    /**
     * @brief Get the string label for a given interval degree.
     *
     * @param degree The interval degree to get the label for.
     * @param out Buffer to write the label to.
     * @param maxOut Maximum size of the output buffer.
     * @return true if successful, false if degree is invalid or no labels attached.
     * @return false
     */
    bool GetIntervalLabel(Degree degree, char *out, std::size_t maxOut) const
    {
      if (degreesPerPeriod_ == 0)
        return false;

      return intervalLabels_.Get(
          static_cast<Degree>(wrap(static_cast<int>(degree),
                                   static_cast<int>(degreesPerPeriod_))),
          out, maxOut);
    }

    /**
     * @brief Get the string label for a given interval degree.
     *
     * @param degree The interval degree to get the label for.
     * @param out Buffer to write the label to.
     * @return true if successful, false if degree is invalid or no labels attached.
     * @return false
     */
    template <std::size_t N>
    bool GetIntervalLabel(Degree degree, MString<N> &out) const
    {
      if (degreesPerPeriod_ == 0)
        return false;

      return intervalLabels_.Get(
          static_cast<Degree>(wrap(static_cast<int>(degree),
                                   static_cast<int>(degreesPerPeriod_))),
          out);
    }

    /**
     * @brief Get the number of degrees per period in this temperament.
     *
     * @return uint16_t Number of degrees per period.
     */
    uint16_t DegreesPerPeriod() const { return degreesPerPeriod_; }

    /**
     * @brief Get the ratio of the period (e.g., 2.0 for octave).
     *
     * @return float The period ratio.
     */
    float PeriodRatio() const { return periodRatio_; }

    /**
     * @brief Get the frequency ratio for a given degree within one period.
     *
     * @param degree The degree to get the ratio for.
     * @return float The frequency ratio (1.0 for unison).
     */
    float RatioAt(Degree degree) const
    {
      if (degreesPerPeriod_ == 0)
        return 1.0f;

      int idx =
          wrap(static_cast<int>(degree), static_cast<int>(degreesPerPeriod_));
      return ratios_[idx];
    }

    /**
     * @brief Get the multiplier for a given period offset.
     *
     * @param period The period offset (e.g., 1 for one octave up).
     * @return float The period multiplier.
     */
    float PeriodMultiplier(Period period) const
    {
      if (period >= MIN_PERIOD && period <= MAX_PERIOD)
        return periodMultipliers_[period - MIN_PERIOD];

      return fastPow(periodRatio_, period);
    }

    /**
     * @brief Get the combined ratio for a degree and period offset.
     *
     * @param degree The degree within the period.
     * @param period The period offset.
     * @return float The combined frequency ratio.
     */
    float RatioAt(int degree, Period period) const
    {
      return RatioAt(degree) * PeriodMultiplier(period);
    }

    /**
     * @brief Calculate frequency from a root frequency, degree, and period.
     *
     * @param rootHz The root frequency in Hz.
     * @param degree The degree within the period.
     * @param period The period offset.
     * @param fineCents Fine tuning adjustment in cents.
     * @return float The calculated frequency in Hz.
     */
    float FrequencyFromRoot(float rootHz, Degree degree, Period period = 0,
                            float fineCents = 0.0f) const
    {
      float ratio = RatioAt(degree, period);
      if (fineCents != 0.0f)
        ratio *= CentsToRatio(fineCents);
      return rootHz * ratio;
    }

    /**
     * @brief Calculate frequency from C0 (16.35 Hz), degree, and period.
     *
     * @param degree The degree within the period.
     * @param period The period offset.
     * @param fineCents Fine tuning adjustment in cents.
     * @return float The calculated frequency in Hz.
     */
    float FrequencyFromC0(Degree degree, Period period = 0,
                          float fineCents = 0.0F) const
    {
      return FrequencyFromRoot(FREQ_C0, degree, period, fineCents);
    }

    /**
     * @brief Calculate frequency from a tuning reference pitch.
     *
     * @param pitch The tempered pitch to calculate.
     * @param ref The tuning reference (root frequency and degree).
     * @return float The calculated frequency in Hz.
     */
    float FrequencyFromReference(const TemperedPitch &pitch,
                                 const TuningReference &ref) const
    {
      int relDegree = pitch.degree - ref.referenceDegree;
      int relPeriod = pitch.period - ref.referencePeriod;

      float ratio = RatioAt(relDegree, relPeriod);

      if (pitch.fineCents != 0.0f)
        ratio *= CentsToRatio(pitch.fineCents);

      return ref.referenceHz * ratio;
    }

    /**
     * @brief Find the nearest degree for a given frequency ratio.
     *
     * @param targetRatio The target frequency ratio.
     * @param outDegree Output parameter for the nearest degree.
     * @param outErrorCents Output parameter for the error in cents.
     * @return true if successful, false if no valid degree found.
     * @return false
     */
    bool FindNearestDegreeForRatio(float targetRatio, Degree &outDegree,
                                   float &outErrorCents) const
    {
      if (degreesPerPeriod_ == 0 || targetRatio <= 0.0f)
        return false;

      while (targetRatio < 1.0f)
        targetRatio *= periodRatio_;
      while (targetRatio >= periodRatio_)
        targetRatio /= periodRatio_;

      Degree best = 0;
      float bestErr = 1e30f;

      for (uint16_t i = 0; i < degreesPerPeriod_; ++i)
      {
        float err = RatioToCents(targetRatio / ratios_[i]);
        float absErr = fabsf(err);
        if (absErr < bestErr)
        {
          bestErr = absErr;
          best = (Degree)i;
          outErrorCents = err;
        }
      }

      outDegree = best;
      return true;
    }

    /**
     * @brief Calculate the minimum distance between two notes in degrees.
     *
     * @param a First note.
     * @param b Second note.
     * @return int The distance in degrees (always positive).
     */
    int GetNoteDistance(Note a, Note b) const
    {
      return abs(b - a) % degreesPerPeriod_;
    }

    /**
     * @brief Check if an interval is considered consonant in this temperament.
     *
     * TODO: This does not support anything but 12EDO
     * @param interval The interval degree to check.
     * @return true if consonant, false otherwise.
     */
    bool IsConsonant(Degree interval) const
    {
      switch (interval % degreesPerPeriod_)
      {
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
    const float FREQ_C0 = 16.35f;

    Kind kind_;
    uint16_t degreesPerPeriod_;
    float periodRatio_;
    float ratios_[MAX_DEGREES];
    float periodMultipliers_[(MAX_PERIOD - MIN_PERIOD) + 1];
    LabelSet<MAX_DEGREES> noteLabels_;
    LabelSet<MAX_DEGREES> intervalLabels_;

    /**
     * @brief Rebuild the cached period multipliers array.
     *
     */
    void RebuildPeriodMultipliers()
    {
      for (Period p = MIN_PERIOD; p <= MAX_PERIOD; ++p)
        periodMultipliers_[p - MIN_PERIOD] = fastPow(periodRatio_, p);
    }

    /**
     * @brief Sort the ratios array in ascending order using insertion sort.
     *
     */
    void SortRatios()
    {
      for (uint16_t i = 1; i < degreesPerPeriod_; ++i)
      {
        float key = ratios_[i];
        int j = (int)i - 1;

        while (j >= 0 && ratios_[j] > key)
        {
          ratios_[j + 1] = ratios_[j];
          --j;
        }

        ratios_[j + 1] = key;
      }
    }
  };

} // namespace Music
