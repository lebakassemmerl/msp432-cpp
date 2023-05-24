// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>

namespace cm4f {
inline uint32_t get_fpscr(void) noexcept
{
    uint32_t ret;
    __asm__ __volatile__(
        "VMRS %0, fpscr"
        : "=r" (ret)
        ::);

    return ret;
}

inline void set_fpscr(uint32_t val) noexcept
{
    __asm__ __volatile__(
        "VMSR fpscr, %0"
        :: "r" (val)
        :  "vfpcc");
}

inline float sqrt(float val) noexcept
{
    __asm__ __volatile__(
        "VSQRT.f32 %0, %1"
        : "=t" (val)
        : "t" (val)
        :);

    return val;
}
}
