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
class Timer32Registers {
public:
    Timer32Registers() = delete;
    Timer32Registers(Timer32Registers&) = delete;
    Timer32Registers(Timer32Registers&&) = delete;
    ~Timer32Registers() = delete;

    ReadWrite<uint32_t> load;
    ReadOnly<uint32_t> value;
    ReadWrite<uint32_t> control;
    WriteOnly<uint32_t> intclr;
    ReadOnly<uint32_t> ris;
    ReadOnly<uint32_t> mis;
    ReadWrite<uint32_t> bgload;
    Reserved<uint32_t> _reserved0[1];
};
#pragma pack()

static_assert(std::is_standard_layout<Timer32Registers>::value,
    "Timer32Registers isn't standard layout");

constexpr size_t TIMER32_1_BASE = 0x4000C000;
constexpr size_t TIMER32_2_BASE = 0x4000C020;

namespace timer32regs {
    namespace control {
        constexpr BitField<uint32_t> oneshot{0, 0};
        constexpr BitField<uint32_t> size{1, 1};
        constexpr BitField<uint32_t> prescale{3, 2};
        constexpr BitField<uint32_t> ie{5, 5};
        constexpr BitField<uint32_t> mode{6, 6};
        constexpr BitField<uint32_t> enable{7, 7};
    }
    namespace ris {
        constexpr BitField<uint32_t> raw_ifg{0, 0};
    }
    namespace mis {
        constexpr BitField<uint32_t> ifg{0, 0};
    }
}
