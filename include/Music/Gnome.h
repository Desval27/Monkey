/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Gnome.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief Pulse-driven meter tracker for bars and beats.
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/NoteValue.h>
#include <Music/TimeSignature.h>

namespace Music
{

  /**
   * @brief Tracks beat, bar, and pulse position for a fixed time signature.
   *
   * `Gnome` advances in pulse-sized steps. A call to `DoPulse()` increments the
   * internal pulse counter and, on beat boundaries, updates the current beat and
   * bar. The object starts in a pre-roll state where `GetBeat()` and
   * `GetPulse()` return `-1`; the first call to `DoPulse()` enters bar `0`,
   * beat `0`, pulse `0`.
   *
   * Edge queries describe the current pulse position:
   * - `RisingBeatEdge()` is true only on the first pulse of each beat.
   * - `RisingBarEdge()` is true only on the first pulse of each bar.
   *
   * Invalid configurations are treated as idle:
   * - `ts.beats <= 0`
   * - `ts.beatValue == NoteValue::None`
   * - `bars <= 0`
   */
  class Gnome
  {
  public:
    /**
     * @brief Constructs a tracker for a finite number of bars.
     *
     * The time signature is referenced, not copied, so it must outlive the
     * tracker.
     *
     * @param ts Time signature that defines the beat size and beat count.
     * @param bars Number of bars before the tracker wraps back to the start.
     */
    Gnome(const TimeSignature &ts, int bars) : ts_(&ts), bars_(bars), bar_(0)
    {
      Reset();
    }

    /**
     * @brief Advances to the next beat boundary.
     *
     * When the final beat of the final bar is exceeded, the tracker wraps back
     * to bar `0`, beat `0`, pulse `0`.
     */
    void DoBeat()
    {
      if (!IsValid())
        return;

      beat_++;
      if (beat_ == ts_->beats)
      {
        beat_ = 0;
        bar_++;
        if (bar_ == bars_)
          bar_ = 0;
      }

      pulse_ = (bar_ * ts_->GetPulsesPerBar()) + (beat_ * PulsesPerBeat());
    }

    /**
     * @brief Advances the tracker by one pulse.
     *
     * On a rising beat edge, the beat and bar counters are updated before the
     * pulse value is returned.
     *
     * @return Current pulse index after the increment, or `-1` when the meter
     * is invalid.
     */
    int DoPulse()
    {
      if (!IsValid())
        return -1;

      pulse_++;
      if (RisingBeatEdge())
      {
        DoBeat();
      }
      return pulse_;
    }

    /**
     * @brief Returns the current beat index.
     *
     * Returns `-1` before the first pulse is processed.
     *
     * @return Zero-based beat index within the current bar.
     */
    int GetBeat() const { return beat_; }

    /**
     * @brief Returns the current bar index.
     *
     * @return Zero-based bar index within the looped span.
     */
    int GetBar() const { return bar_; }

    /**
     * @brief Returns the current pulse index.
     *
     * Returns `-1` before the first pulse is processed.
     *
     * @return Zero-based pulse index within the looped span.
     */
    int GetPulse() const { return pulse_; }

    /**
     * @brief Returns true only on the first pulse of a beat.
     *
     * @return `true` when the current pulse is aligned to the beat grid.
     */
    bool RisingBeatEdge() const
    {
      const int pulsesPerBeat = PulsesPerBeat();
      return (pulsesPerBeat > 0) && (pulse_ >= 0) &&
             ((pulse_ % pulsesPerBeat) == 0);
    }

    /**
     * @brief Returns true only on the first pulse of a bar.
     *
     * Unlike checking `GetBeat() == 0`, this is a one-pulse edge and is false
     * for the remaining pulses inside beat zero.
     *
     * @return `true` when the current pulse is the first pulse of a bar.
     */
    bool RisingBarEdge() const
    {
      return RisingBeatEdge() && (beat_ == 0);
    }

    /**
     * @brief Resets the tracker to the pre-roll state.
     *
     * After `Reset()`, the next call to `DoPulse()` produces the first pulse of
     * bar `0`, beat `0`.
     */
    void Reset()
    {
      beat_ = -1;
      bar_ = 0;
      pulse_ = -1;
    }

  private:
    const TimeSignature *ts_;
    int bars_;
    int beat_;
    int bar_;
    int pulse_;

    int PulsesPerBeat() const
    {
      return static_cast<int>(ts_->beatValue);
    }

    bool IsValid() const
    {
      return ts_ && (ts_->beats > 0) && (PulsesPerBeat() > 0) && (bars_ > 0);
    }
  };

} // namespace Music
