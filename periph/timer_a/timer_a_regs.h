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
class TimerARegisters {
public:
    TimerARegisters() = delete;
    TimerARegisters(TimerARegisters&) = delete;
    TimerARegisters(TimerARegisters&&) = delete;
    ~TimerARegisters() = delete;

    ReadWrite<uint16_t> ctl;
    ReadWrite<uint16_t> cctlx[5];
    Reserved<uint16_t> _reserved0[2];
    ReadWrite<uint16_t> r;
    ReadWrite<uint16_t> ccrx[5];
    Reserved<uint16_t> _reserved1[2];
    ReadWrite<uint16_t> ex0;
    Reserved<uint16_t> _reserved2[6];
    ReadOnly<uint16_t> iv;
};
#pragma pack()

static_assert(std::is_standard_layout<TimerARegisters>::value, "TimerARegisters isn't standard layout");

constexpr size_t TIMER_A0_BASE = 0x40000000;
constexpr size_t TIMER_A1_BASE = 0x40000400;
constexpr size_t TIMER_A2_BASE = 0x40000800;
constexpr size_t TIMER_A3_BASE = 0x40000C00;

namespace timeraregs {
    namespace ctl {
        constexpr BitField<uint16_t> ifg{0, 0};
        constexpr BitField<uint16_t> ie{1, 1};
        constexpr BitField<uint16_t> clr{2, 2};
        constexpr BitField<uint16_t> mc{5, 4};
        constexpr BitField<uint16_t> id{7, 6};
        constexpr BitField<uint16_t> ssel{9, 8};
    }
    namespace ex0 {
        constexpr BitField<uint16_t> idex{2, 0};
    }
    namespace cctl {
        constexpr BitField<uint16_t> ccifg{0, 0};
        constexpr BitField<uint16_t> cov{1, 1};
        constexpr BitField<uint16_t> out{2, 2};
        constexpr BitField<uint16_t> cci{3, 3};
        constexpr BitField<uint16_t> ccie{4, 4};
        constexpr BitField<uint16_t> outmod{7, 5};
        constexpr BitField<uint16_t> cap{8, 8};
        constexpr BitField<uint16_t> scci{10, 10};
        constexpr BitField<uint16_t> scs{11, 11};
        constexpr BitField<uint16_t> ccis{13, 12};
        constexpr BitField<uint16_t> cm{15, 14};
    }
}
