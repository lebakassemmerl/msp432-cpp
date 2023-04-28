// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <type_traits>
#include <cstddef>
#include <cstdint>

#include "helpers.h"
#include "register.h"

#pragma pack(1)
class WdtRegisters {
public:
    WdtRegisters() = delete;
    WdtRegisters(WdtRegisters&) = delete;
    WdtRegisters(WdtRegisters&&) = delete;
    ~WdtRegisters() = delete;

    Reserved<uint16_t> _reserved0[6];
    ReadWrite<uint16_t> ctl;
};
#pragma pack()

static_assert(std::is_standard_layout<WdtRegisters>::value, "WdtRegisters isn't standard layout");

constexpr size_t WDT_BASE = 0x40004800;

namespace wdtregs {
    namespace ctl {
        constexpr uint16_t KEY_WRITE = 0x5A;
        constexpr uint16_t KEY_READ = 0x69;
        constexpr BitField<uint16_t> is(2, 0);
        constexpr BitField<uint16_t> cntcl(3, 3);
        constexpr BitField<uint16_t> tmsel(4, 4);
        constexpr BitField<uint16_t> ssel(6, 5);
        constexpr BitField<uint16_t> hold(7, 7);
        constexpr BitField<uint16_t> pw(15, 8);
    }
}
