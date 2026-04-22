/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file TTY.h
 * @brief TTY Color Definitions
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

#define TTY_RESET       "\033[0m"
#define TTY_BOLD        "\033[1m"

#define TTY_FG_BLACK    "\033[30m"
#define TTY_FG_RED      "\033[31m"
#define TTY_FG_GREEN    "\033[32m"
#define TTY_FG_YELLOW   "\033[33m"
#define TTY_FG_BLUE     "\033[34m"
#define TTY_FG_MAGENTA  "\033[35m"
#define TTY_FG_CYAN     "\033[36m"
#define TTY_FG_WHITE    "\033[37m"

#define TTY_BG_BLACK    "\033[40m"
#define TTY_BG_RED      "\033[41m"
#define TTY_BG_GREEN    "\033[42m"
#define TTY_BG_YELLOW   "\033[43m"
#define TTY_BG_BLUE     "\033[44m"
#define TTY_BG_MAGENTA  "\033[45m"
#define TTY_BG_CYAN     "\033[46m"
#define TTY_BG_WHITE    "\033[47m"
