/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Labels.h
 * @author pfburdette <paul.f.burdette@gmail.com>
 * @brief Potentially useful music related strings
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
*/
#pragma once

namespace Music
{
static const char* const NOTE_NAMES_12[] = {"C",
                                            "C#/Db",
                                            "D",
                                            "D#/Eb",
                                            "E",
                                            "F",
                                            "F#/Gb",
                                            "G",
                                            "G#/Ab",
                                            "A",
                                            "A#/Bb",
                                            "B"};

static const char* const INTERVAL_NAMES_12[]
    = {"P1", "m2", "M2", "m3", "M3", "P4", "TT", "P5", "m6", "M6", "m7", "M7"};

static const char* const NOTE_NAMES_15[] = {"C",
                                            "C#/Db",
                                            "D",
                                            "D#",
                                            "Eb",
                                            "E",
                                            "F",
                                            "F#/Gb",
                                            "G",
                                            "G#",
                                            "Ab",
                                            "A",
                                            "A#",
                                            "Bb",
                                            "B"};

static const char* const INTERVAL_NAMES_15[] = {"P1",
                                                "m2",
                                                "M2",
                                                "A2",
                                                "m3",
                                                "M3",
                                                "P4",
                                                "TT",
                                                "P5",
                                                "A5",
                                                "m6",
                                                "M6",
                                                "A6",
                                                "m7",
                                                "M7"};
}; // namespace Music