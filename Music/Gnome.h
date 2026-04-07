/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Chord.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief Chord definitions and voicing.
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
*/
#pragma once

#include <Music/Music.h>

namespace Music
{

class Gnome
{
  public:
    Gnome(TimeSignature& ts, int bars) : ts(ts), bars(bars) 
    {
        reset();
        beat = -1;  // Not running yet.
        pulse = -1;
    }

    /**
     * @brief Advances to the next beat
     * 
     */
    void doBeat()
    {
        beat++;
        if(beat == ts.beats)
        {
            bar++;
            if (bar == bars)
            {
                reset();
            }
            else 
            {
                beat = 0;
            }
        }
    }

    int doPulse() 
    {
        pulse++;
        if (RisingBeatEdge())
        {
            doBeat();
        }
        return pulse;
    }

    /**
     * @brief Returns the current beat
     * 
     * @return int 
     */
    int getBeat() const { return beat; }
    /**
     * @brief Returns the current bar
     * 
     * @return int 
     */
    int getBar() const { return bar; }
    /**
     * @brief Returns the current pulse count
     * 
     * @return int 
     */
    int getPulse() const { return pulse; }

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */
    bool RisingBeatEdge() const { return (pulse % static_cast<int>(ts.beatValue)) == 0; }

    /**
     * @brief 
     * 
     * @return true 
     * @return false 
     */

    bool RisingBarEdge() const { return (beat == 0); };

    /**
     * @brief Resets the current bar and beat to one.
     * 
     */
    void reset()
    {
        beat  = 0;
        bar   = 0;
        pulse = 0;
    }

  private:
    TimeSignature ts;
    int           bars;
    int           beat;
    int           bar;
    int           pulse;
};


} // namespace Music
