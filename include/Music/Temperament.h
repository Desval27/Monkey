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

#include <cstring>
#include <Music/Music.h>

namespace Music
{

  /**
   * @brief
   *
   */
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

    bool Get(Degree index, char *out, size_t maxOut) const
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
   * @brief
   *
   */
  class Temperament
  {
  public:
    enum class Kind : uint8_t
    {
      RatioTable = 0,
      EqualDivision
    };

    /**
     * @brief Construct a new Temperament object
     *
     */
    Temperament()
    {
      Clear();
      // Default to 12-EDO with octaves at each frequency doubling.
      MakeEqualDivision();
    }

    /**
     * @brief
     *
     */
    void Clear()
    {
      kind_ = Kind::RatioTable;
      degreesPerPeriod_ = 0;
      periodRatio_ = 2.0f;

      // Defaults to no ratios
      for (size_t i = 0; i < MAX_DEGREES; ++i)
        ratios_[i] = 1.0f;

      RebuildPeriodMultipliers();
    }

    /**
     * @brief
     *
     * @param degreesPerPeriod
     * @param periodRatio
     * @return true
     * @return false
     */
    bool MakeEqualDivision(uint16_t degreesPerPeriod = 12,
                           float periodRatio = 2.0f)
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
     * @brief
     *
     * @param ratios
     * @param count
     * @param periodRatio
     * @return true
     * @return false
     */
    bool MakeRatioTable(const float *ratios, uint16_t count,
                        float periodRatio = 2.0f)
    {
      if (!ratios || count == 0 || count > MAX_DEGREES || periodRatio <= 0.0f)
        return false;

      kind_ = Kind::RatioTable;
      degreesPerPeriod_ = count;
      periodRatio_ = periodRatio;

      for (uint16_t i = 0; i < count; ++i)
        ratios_[i] = ratios[i];

      RebuildPeriodMultipliers();
      return true;
    }

    /**
     * @brief
     *
     * @param ratios
     * @param count
     * @param periodRatio
     * @return true
     * @return false
     */
    bool MakeNormalizedRatioTable(const float *ratios, uint16_t count,
                                  float periodRatio = 2.0f)
    {
      if (!ratios || count == 0 || count > MAX_DEGREES || periodRatio <= 1.0f)
        return false;

      kind_ = Kind::RatioTable;
      degreesPerPeriod_ = count;
      periodRatio_ = periodRatio;

      for (uint16_t i = 0; i < count; ++i)
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
     * @brief
     *
     * @param labels
     * @param count
     * @return true
     * @return false
     */
    bool AttachNoteLabels(const char *const *labels, uint16_t count)
    {
      return noteLabels_.Attach(labels, count);
    }

    template <typename T, std::size_t N>
    bool AttachNoteLabels(const std::array<T, N>& labels)
    {
      return noteLabels_.Attach(labels.data(), labels.size());
    }

    /**
     * @brief
     *
     * @param labels
     * @param count
     * @return true
     * @return false
     */
    bool AttachIntervalLabels(const char *const *labels, uint16_t count)
    {
      return intervalLabels_.Attach(labels, count);
    }

    template <typename T, std::size_t N>
    bool AttachIntervalLabels(const std::array<T, N>& labels)
    {
      return intervalLabels_.Attach(labels.data(), labels.size());
    }

    /**
     * @brief Get the Note Label object
     *
     * @param degree
     * @param out
     * @param maxOut
     * @return true
     * @return false
     */
    bool GetNoteLabel(Degree degree, char *out, size_t maxOut) const
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
     * @brief
     *
     * @param degree
     * @param out
     * @param maxOut
     * @return true
     * @return false
     */
    bool GetIntervalLabel(Degree degree, char *out, size_t maxOut) const
    {
      if (degreesPerPeriod_ == 0)
        return false;

      return intervalLabels_.Get(
          static_cast<Degree>(wrap(static_cast<int>(degree),
                                   static_cast<int>(degreesPerPeriod_))),
          out, maxOut);
    }

    /**
     * @brief
     *
     * @return uint16_t
     */
    uint16_t DegreesPerPeriod() const { return degreesPerPeriod_; }

    /**
     * @brief
     *
     * @return float
     */
    float PeriodRatio() const { return periodRatio_; }

    /**
     * @brief
     *
     * @param degree
     * @return float
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
     * @brief
     *
     * @param period
     * @return float
     */
    float PeriodMultiplier(Period period) const
    {
      if (period >= MIN_PERIOD && period <= MAX_PERIOD)
        return periodMultipliers_[period - MIN_PERIOD];

      return fastPow(periodRatio_, period);
    }

    /**
     * @brief
     *
     * @param degree
     * @param period
     * @return float
     */
    float RatioAt(int degree, Period period) const
    {
      return RatioAt(degree) * PeriodMultiplier(period);
    }

    /**
     * @brief
     *
     * @param rootHz
     * @param degree
     * @param period
     * @param fineCents
     * @return float
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
     * @brief
     *
     * @param degree
     * @param period
     * @param fineCents
     * @return float
     */
    float FrequencyFromC0(Degree degree, Period period = 0,
                          float fineCents = 0.0F) const
    {
      return FrequencyFromRoot(FREQ_C0, degree, period, fineCents);
    }

    /**
     * @brief
     *
     * @param pitch
     * @param ref
     * @return float
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
     * @brief
     *
     * @param targetRatio
     * @param outDegree
     * @param outErrorCents
     * @return true
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
     * @brief
     */
    int GetNoteDistance(Note a, Note b) const
    {
      return abs(b - a) % degreesPerPeriod_;
    }

    /**
     * @brief
     *
     * TODO: This does not support anything but 12EDO
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
    LabelSet noteLabels_;
    LabelSet intervalLabels_;

    /**
     * @brief
     *
     */
    void RebuildPeriodMultipliers()
    {
      for (Period p = MIN_PERIOD; p <= MAX_PERIOD; ++p)
        periodMultipliers_[p - MIN_PERIOD] = fastPow(periodRatio_, p);
    }

    /**
     * @brief
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
