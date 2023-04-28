// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <type_traits>
#include <cstddef>

#include "helpers.h"
#include "register.h"

#pragma pack(1)
class NvicRegisters {
public:
    NvicRegisters() = delete;
    NvicRegisters(NvicRegisters&) = delete;
    NvicRegisters(NvicRegisters&&) = delete;
    ~NvicRegisters() = delete;

    ReadWrite<uint32_t> iser0;
    ReadWrite<uint32_t> iser1;
    Reserved<uint32_t> _reserved3[30];
    ReadWrite<uint32_t> icer0;
    ReadWrite<uint32_t> icer1;
    Reserved<uint32_t> _reserved4[30];
    ReadWrite<uint32_t> ispr0;
    ReadWrite<uint32_t> ispr1;
    Reserved<uint32_t> _reserved5[30];
    ReadWrite<uint32_t> icpr0;
    ReadWrite<uint32_t> icpr1;
    Reserved<uint32_t> _reserved6[30];
    ReadOnly<uint32_t> iabr0;
    ReadOnly<uint32_t> iabr1;
    Reserved<uint32_t> _reserved7[62];
    ReadWrite<uint32_t> ipr[16];
};

static_assert(std::is_standard_layout<NvicRegisters>::value, "NvicRegisters isn't standard layout");

constexpr size_t NVIC_BASE = 0xE000E100;

namespace nvicregs{
    namespace ipr {
        constexpr BitField<uint32_t> pri0(7, 0);
        constexpr BitField<uint32_t> pri1(15, 8);
        constexpr BitField<uint32_t> pri2(23, 16);
        constexpr BitField<uint32_t> pri3(31, 24);
    }
}