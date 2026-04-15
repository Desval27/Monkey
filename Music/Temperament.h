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

#include <Music/MusicHelpers.h>
#include <Music/MusicTypes.h>

namespace Music {

/**
 * @brief
 *
 */
class LabelSet {
 public:
  LabelSet() : _count(0), _labels(nullptr) {}

  bool Attach(const char* const* labels, uint16_t count) {
    if (!labels || count == 0 || count > MAX_DEGREES) return false;

    _labels = labels;
    _count = count;
    return true;
  }

  bool Get(Degree index, char* out, size_t maxOut) const {
    if (!_labels || index >= _count || !out || maxOut == 0) return false;

    const char* src = TUNING_PGM_READ_PTR(&_labels[index]);
    tuning_copy_string(out, src, maxOut);
    return true;
  }

 private:
  uint16_t _count;
  const char* const* _labels;
};

/**
 * @brief
 *
 */
class Temperament {
 public:
  enum class Kind : uint8_t { RatioTable = 0, EqualDivision };

  /**
   * @brief Construct a new Temperament object
   *
   */
  Temperament() {
    Clear();
    // Default to 12-EDO with octaves at each frequency doubling.
    MakeEqualDivision();
  }

  /**
   * @brief
   *
   */
  void Clear() {
    _kind = Kind::RatioTable;
    _degreesPerPeriod = 0;
    _periodRatio = 2.0f;

    // Defaults to no ratios
    for (size_t i = 0; i < MAX_DEGREES; ++i) _ratios[i] = 1.0f;

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
                         float periodRatio = 2.0f) {
    if (degreesPerPeriod == 0 || degreesPerPeriod > MAX_DEGREES ||
        periodRatio <= 0.0f)
      return false;

    _kind = Kind::EqualDivision;
    _degreesPerPeriod = degreesPerPeriod;
    _periodRatio = periodRatio;

    for (uint16_t i = 0; i < _degreesPerPeriod; ++i)
      _ratios[i] = powf(_periodRatio, (float)i / (float)_degreesPerPeriod);

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
  bool MakeRatioTable(const float* ratios, uint16_t count,
                      float periodRatio = 2.0f) {
    if (!ratios || count == 0 || count > MAX_DEGREES || periodRatio <= 0.0f)
      return false;

    _kind = Kind::RatioTable;
    _degreesPerPeriod = count;
    _periodRatio = periodRatio;

    for (uint16_t i = 0; i < count; ++i) _ratios[i] = ratios[i];

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
  bool MakeNormalizedRatioTable(const float* ratios, uint16_t count,
                                float periodRatio = 2.0f) {
    if (!ratios || count == 0 || count > MAX_DEGREES || periodRatio <= 1.0f)
      return false;

    _kind = Kind::RatioTable;
    _degreesPerPeriod = count;
    _periodRatio = periodRatio;

    for (uint16_t i = 0; i < count; ++i) {
      float r = ratios[i];
      if (r <= 0.0f) return false;

      while (r < 1.0f) r *= _periodRatio;
      while (r >= _periodRatio) r /= _periodRatio;

      _ratios[i] = r;
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
  bool AttachNoteLabels(const char* const* labels, uint16_t count) {
    return _noteLabels.Attach(labels, count);
  }

  /**
   * @brief
   *
   * @param labels
   * @param count
   * @return true
   * @return false
   */
  bool AttachIntervalLabels(const char* const* labels, uint16_t count) {
    return _intervalLabels.Attach(labels, count);
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
  bool GetNoteLabel(Degree degree, char* out, size_t maxOut) const {
    if (_degreesPerPeriod == 0) return false;

    return _noteLabels.Get(
        static_cast<Degree>(wrap(static_cast<int>(degree),
                                 static_cast<int>(_degreesPerPeriod))),
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
  bool GetIntervalLabel(Degree degree, char* out, size_t maxOut) const {
    if (_degreesPerPeriod == 0) return false;

    return _intervalLabels.Get(
        static_cast<Degree>(wrap(static_cast<int>(degree),
                                 static_cast<int>(_degreesPerPeriod))),
        out, maxOut);
  }

  /**
   * @brief 
   * 
   * @return uint16_t 
   */
  uint16_t DegreesPerPeriod() const { return _degreesPerPeriod; }

  /**
   * @brief 
   * 
   * @return float 
   */
  float PeriodRatio() const { return _periodRatio; }

  /**
   * @brief 
   * 
   * @param degree 
   * @return float 
   */
  float RatioAt(Degree degree) const {
    if (_degreesPerPeriod == 0) return 1.0f;

    int idx =
        wrap(static_cast<int>(degree), static_cast<int>(_degreesPerPeriod));
    return _ratios[idx];
  }

  /**
   * @brief 
   * 
   * @param period 
   * @return float 
   */
  float PeriodMultiplier(Period period) const {
    if (period >= MIN_PERIOD && period <= MAX_PERIOD)
      return _periodMultipliers[period - MIN_PERIOD];

    return fastPow(_periodRatio, period);
  }

  /**
   * @brief 
   * 
   * @param degree 
   * @param period 
   * @return float 
   */
  float RatioAt(int degree, Period period) const {
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
                          float fineCents = 0.0f) const {
    float ratio = RatioAt(degree, period);
    if (fineCents != 0.0f) ratio *= CentsToRatio(fineCents);
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
                        float fineCents = 0.0F) const {
    return FrequencyFromRoot(FREQ_C0, degree, period, fineCents);
  }

  /**
   * @brief 
   * 
   * @param pitch 
   * @param ref 
   * @return float 
   */
  float FrequencyFromReference(const TemperedPitch& pitch,
                               const TuningReference& ref) const {
    int relDegree = pitch.degree - ref.referenceDegree;
    int relPeriod = pitch.period - ref.referencePeriod;

    float ratio = RatioAt(relDegree, relPeriod);

    if (pitch.fineCents != 0.0f) ratio *= CentsToRatio(pitch.fineCents);

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
  bool FindNearestDegreeForRatio(float targetRatio, Degree& outDegree,
                                 float& outErrorCents) const {
    if (_degreesPerPeriod == 0 || targetRatio <= 0.0f) return false;

    while (targetRatio < 1.0f) targetRatio *= _periodRatio;
    while (targetRatio >= _periodRatio) targetRatio /= _periodRatio;

    Degree best = 0;
    float bestErr = 1e30f;

    for (uint16_t i = 0; i < _degreesPerPeriod; ++i) {
      float err = RatioToCents(targetRatio / _ratios[i]);
      float absErr = fabsf(err);
      if (absErr < bestErr) {
        bestErr = absErr;
        best = (Degree)i;
        outErrorCents = err;
      }
    }

    outDegree = best;
    return true;
  }

 private:
  const float FREQ_C0 = 16.35f;

  Kind _kind;
  uint16_t _degreesPerPeriod;
  float _periodRatio;
  float _ratios[MAX_DEGREES];
  float _periodMultipliers[(MAX_PERIOD - MIN_PERIOD) + 1];
  LabelSet _noteLabels;
  LabelSet _intervalLabels;

  /**
   * @brief 
   * 
   */
  void RebuildPeriodMultipliers() {
    for (Period p = MIN_PERIOD; p <= MAX_PERIOD; ++p)
      _periodMultipliers[p - MIN_PERIOD] = fastPow(_periodRatio, p);
  }

  /**
   * @brief 
   * 
   */
  void SortRatios() {
    for (uint16_t i = 1; i < _degreesPerPeriod; ++i) {
      float key = _ratios[i];
      int j = (int)i - 1;

      while (j >= 0 && _ratios[j] > key) {
        _ratios[j + 1] = _ratios[j];
        --j;
      }

      _ratios[j + 1] = key;
    }
  }
};

}  // namespace Music
