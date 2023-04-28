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
class SystickRegisters {
public:
    SystickRegisters() = delete;
    SystickRegisters(SystickRegisters&) = delete;
    SystickRegisters(SystickRegisters&&) = delete;
    ~SystickRegisters() = delete;

    ReadWrite<uint32_t> stcsr;
    ReadWrite<uint32_t> strvr;
    ReadWrite<uint32_t> stcvr;
    ReadOnly<uint32_t> stcr;
};
#pragma pack()

static_assert(std::is_standard_layout<SystickRegisters>::value, 
    "SystickRegisters isn't standard layout");

constexpr size_t SYSTICK_BASE = 0xE000E010;

namespace systickregs {
    namespace stcsr {
        constexpr BitField<uint32_t> enable{0, 0};
        constexpr BitField<uint32_t> tickint{1, 1};
        constexpr BitField<uint32_t> clksrc{2, 2};
        constexpr BitField<uint32_t> countflag{16, 16};
    }
    namespace strvr {
        constexpr BitField<uint32_t> reload{23, 0};
    }
    namespace stcvr {
        constexpr BitField<uint32_t> current{23, 0};
    }
    namespace stcr {
        constexpr BitField<uint32_t> tenms{23, 0};
        constexpr BitField<uint32_t> skew{30, 30};
        constexpr BitField<uint32_t> noref{31, 31};
    }
}