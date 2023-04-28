// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>

namespace cm4f {
inline uint32_t get_fpscr(void)
{
    uint32_t ret;
    asm volatile (
        "VMRS %0, fpscr"
        : "=r" (ret)
        ::);

    return ret;
}

inline void set_fpscr(uint32_t val)
{
    asm volatile (
        "VMSR fpscr, %0"
        :: "r" (val)
        :  "vfpcc");
}
}
