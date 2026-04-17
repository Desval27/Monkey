/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Gnome.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <Music/Music.h>

namespace Music
{

  /**
   * @brief
   *
   */
  class Gnome
  {
  public:
    /**
     * @brief
     *
     * @param ts
     * @param bars
     */
    Gnome(const TimeSignature &ts, int bars) : _ts(ts), _bars(bars)
    {
      Reset();
      _beat = -1; // Not running yet.
      _pulse = -1;
    }

    /**
     * @brief Advances to the next beat
     *
     */
    void DoBeat()
    {
      _beat++;
      if (_beat == _ts.beats)
      {
        _bar++;
        if (_bar == _bars)
        {
          Reset();
        }
        else
        {
          _beat = 0;
        }
      }
    }

    /**
     * @brief
     *
     * @return int
     */
    int DoPulse()
    {
      _pulse++;
      if (RisingBeatEdge())
      {
        DoBeat();
      }
      return _pulse;
    }

    /**
     * @brief Returns the current beat
     *
     * @return int
     */
    int GetBeat() const { return _beat; }

    /**
     * @brief Returns the current bar
     *
     * @return int
     */
    int GetBar() const { return _bar; }

    /**
     * @brief Returns the current pulse count
     *
     * @return int
     */
    int GetPulse() const { return _pulse; }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool RisingBeatEdge() const
    {
      return (_pulse % static_cast<int>(_ts.beatValue)) == 0;
    }

    /**
     * @brief
     *
     * @return true
     * @return false
     */
    bool RisingBarEdge() const { return (_beat == 0); };

    /**
     * @brief Resets the current bar and beat to one.
     *
     */
    void Reset()
    {
      _beat = 0;
      _bar = 0;
      _pulse = 0;
    }

  private:
    const TimeSignature _ts;
    int _bars;
    int _beat;
    int _bar;
    int _pulse;
  };

} // namespace Music
