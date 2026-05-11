/* SPDX-License-Identifier: CC0-1.0 */
/**
 * @file Singleton.h
 * @brief
 * @author pfburdette <paul.f.burdette@gmail.com>
 *
 * @copyright This work is dedicated to the public domain under CC0 1.0.
 * To the extent possible under law, the author(s) have waived all copyright
 * and related or neighboring rights to this software.
 * See <http://creativecommons.org>
 */
#pragma once

template<typename TEventType>
class Singleton
{
public:
  static auto get_instance() -> TEventType&
  {
    static TEventType instance;
    return instance;
  }

protected:
  Singleton() = default;
};