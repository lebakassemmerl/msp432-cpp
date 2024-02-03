// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>
#include <cstddef>

#include <array>
#include <expected>

#include "err.h"
#include "helpers.h"

template<size_t N>
class Bitmap {
public:
    static_assert(hlp::is_powerof2<N>(), "Bitmap: N must be a power of 2");

    constexpr explicit Bitmap() noexcept : bits() {}
    constexpr ~Bitmap() noexcept {}

    Err set(size_t idx) noexcept
    {
        if (idx >= N)
            return Err::OutOfRange;

        bits[idx / BITS_PER_WORD] |= ONE << (static_cast<T>(idx) & MASK);
        return Err::Ok;
    }

    Err clear(size_t idx) noexcept
    {
        if (idx >= N)
            return Err::OutOfRange;

        bits[idx / BITS_PER_WORD] &= ~(ONE << (static_cast<T>(idx) & MASK));
        return Err::Ok;
    }

    std::expected<bool, Err> test(size_t idx) const noexcept
    {
        if (idx >= N)
            return std::unexpected{Err::OutOfRange};

        bool ret = (bits[idx / BITS_PER_WORD] & (ONE << (static_cast<T>(idx) & MASK))) > 0;
        return std::expected<bool, Err>{ret};
    }

private:
    using T = unsigned long;
    static constexpr size_t BITS_PER_WORD = sizeof(T) * 8;
    static constexpr T ONE = static_cast<T>(1);
    static constexpr T MASK = static_cast<T>(BITS_PER_WORD) - ONE;

    std::array<T, (N + BITS_PER_WORD - 1) / BITS_PER_WORD> bits;
};
