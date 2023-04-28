// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <type_traits>
#include <cstddef>
#include "register.h"

#pragma pack(1)
class GpioRegisters {
public:
    GpioRegisters() = delete;
    GpioRegisters(GpioRegisters&) = delete;
    GpioRegisters(GpioRegisters&&) = delete;
    ~GpioRegisters() = delete;

    ReadOnly<uint8_t> in[2];
    ReadWrite<uint8_t> out[2];
    ReadWrite<uint8_t> dir[2];
    ReadWrite<uint8_t> ren[2];
    ReadWrite<uint8_t> ds[2];
    ReadWrite<uint8_t> sel0[2];
    ReadWrite<uint8_t> sel1[2];
    ReadWrite<uint8_t> iv1[2];
    Reserved<uint8_t> _reserved[6];
    ReadWrite<uint8_t> selc[2];
    ReadWrite<uint8_t> ies[2];
    ReadWrite<uint8_t> ifg[2];
    ReadWrite<uint8_t> iv2[2];
};
#pragma pack()

static_assert(std::is_standard_layout<GpioRegisters>::value, "GpioRegisters isn't standard layout");

constexpr size_t GPIO_BASES[6] = {
    0x40004C00,
    0x40004C20,
    0x40004C40,
    0x40004C60,
    0x40004C80,
    0x40004D20,
};