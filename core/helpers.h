// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <concepts>
#include <cstdint>
#include <cstddef>
#include <span>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

namespace hlp {
template<typename T> requires std::unsigned_integral<T>
constexpr T bit(T pos)
{
    return (static_cast<T>(1) << pos);
}

template<typename T, typename V> requires std::unsigned_integral<T> && std::integral<V>
constexpr T leftshift(V val, T pos)
{
    return (static_cast<T>(val) << pos);
}

template<typename T> requires std::unsigned_integral<T>
constexpr T mask(size_t bit_high, size_t bit_low)
{
    T one = static_cast<T>(1U);
    T step1 = ~static_cast<T>(0);
    T step2 = step1 - static_cast<T>(((one << bit_low) - one));
    T step3 = step1 >> (((sizeof(T) * 8) - one) - bit_high);
    return step2 & step3;
}

template<size_t N>
constexpr bool is_powerof2() noexcept
{
    return N && ((N & (N - 1)) == 0);
}

template<typename T> requires std::integral<T>
constexpr size_t to_hex(std::span<char> text, T val) noexcept
{
    const char LOOKUP[] = "0123456789ABCDEF";

    size_t chars = std::min(text.size(), sizeof(T) * 2);
    for (size_t i = chars; i > 0; i--, val >>= 4)
        text[i - 1] = LOOKUP[val & 0x0F];

    return chars;
}
}
