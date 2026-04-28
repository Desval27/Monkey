
/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file MusicConfig.h
 * @brief 
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#include <cstddef>

namespace Music
{
    // Default limits for template parameters. These can be overridden by defining them before including this header.
    constexpr std::size_t DEF_MAX_DEGREES = 64;
    constexpr std::size_t DEF_MAX_EVENTS = 128;
    constexpr std::size_t DEF_SCALE_DEGREES = 7;  // HEPATONIC

} // namespace Music
