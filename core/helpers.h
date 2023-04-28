// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <concepts>
#include <cstdint>
#include <cstddef>

namespace hlp {
template <typename T> requires std::unsigned_integral<T>
constexpr T bit(T pos)
{
    return (static_cast<T>(1) << pos);
}

template <typename T, typename V> requires std::unsigned_integral<T> && std::integral<V>
constexpr T leftshift(V val, T pos)
{
    return (static_cast<T>(val) << pos);
}

template <typename T> requires std::unsigned_integral<T>
constexpr T mask(size_t bit_high, size_t bit_low)
{
    T step1 = ~static_cast<T>(0);
    T step2 = step1 - ((static_cast<T>(1) << bit_low) - 1);
    T step3 = step1 >> (((sizeof(T) * 8) - 1) - bit_high);
    return step2 & step3;
}

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
}
