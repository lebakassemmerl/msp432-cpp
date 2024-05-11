// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include "libc.h"
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

template<typename T> requires std::integral<T>
constexpr size_t max_digits()
{
    if constexpr (sizeof(T) == 1) {
        return 3;
    } else if constexpr (sizeof(T) == 2) {
        return 5;
    } else if constexpr (sizeof(T) == 4) {
        return 10;
    } else if constexpr ((sizeof(T) == 8) && std::is_signed_v<T>) {
        return 19;
    } else if constexpr ((sizeof(T) == 8) && std::is_unsigned_v<T>) {
        return 20;
    } else {
        return 0;
    }
}

template<typename T> requires std::signed_integral<T>
constexpr void int_to_str(T val, char* text, bool fill_with_zeros = false) noexcept
{
    bool negative;
    size_t idx = max_digits<T>();

    if (val < 0) {
        val = -val;
        negative = true;
    } else {
        negative = false;
    }

    if (fill_with_zeros)
        libc::memset(&text[1], '0', max_digits<T>());
    else
        libc::memset(&text[1], ' ', max_digits<T>());

    while (val > 0) {
        text[idx] = static_cast<char>(val % 10) + '0';
        val /= 10;
        idx--;
    }

    if (negative) {
        if (fill_with_zeros)
            text[0] = '-';
        else
            text[idx] = '-';
    }
}

template<typename T> requires std::unsigned_integral<T>
constexpr void uint_to_str(T val, char* text, bool fill_with_zeros = false) noexcept
{
    size_t idx = max_digits<T>() - 1;

    if (fill_with_zeros)
        libc::memset(&text[1], '0', max_digits<T>());
    else
        libc::memset(&text[1], ' ', max_digits<T>());

    while (val > 0) {
        text[idx] = static_cast<char>(val % 10) + '0';
        val /= 10;
        idx--;
    }
}
}
