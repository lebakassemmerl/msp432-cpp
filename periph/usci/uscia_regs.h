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
class UsciARegisters {
public:
    UsciARegisters() = delete;
    UsciARegisters(UsciARegisters&) = delete;
    UsciARegisters(UsciARegisters&&) = delete;
    ~UsciARegisters() = delete;

    ReadWrite<uint16_t> ctlw0;
    ReadWrite<uint16_t> ctlw1;
    Reserved<uint16_t> _reserved0[1];
    ReadWrite<uint16_t> brw;
    ReadWrite<uint16_t> mctlw;
    ReadWrite<uint16_t> statw;
    ReadOnly<uint16_t> rxbuf;
    ReadWrite<uint16_t> txbuf;
    ReadWrite<uint16_t> abctl;
    ReadWrite<uint16_t> irctl;
    Reserved<uint16_t> _reserved1[3];
    ReadWrite<uint16_t> ie;
    ReadWrite<uint16_t> ifg;
    ReadOnly<uint16_t> iv;
};
#pragma pack()

static_assert(std::is_standard_layout<UsciARegisters>::value, 
    "UsciARegisters isn't standard layout");

constexpr size_t USCIA0_BASE = 0x40001000;
constexpr size_t USCIA1_BASE = 0x40001400;
constexpr size_t USCIA2_BASE = 0x40001800;
constexpr size_t USCIA3_BASE = 0x40001C00;

namespace usciaregs {
    namespace ctlw0 {
        constexpr BitField<uint16_t> swrst{0, 0};
        constexpr BitField<uint16_t> txbrk{1, 1};
        constexpr BitField<uint16_t> txaddr{2, 2};
        constexpr BitField<uint16_t> dorm{3, 3};
        constexpr BitField<uint16_t> brkie{4, 4};
        constexpr BitField<uint16_t> rxeie{5, 5};
        constexpr BitField<uint16_t> ssel{7, 6};
        constexpr BitField<uint16_t> sync{8, 8};
        constexpr BitField<uint16_t> mode{10, 9};
        constexpr BitField<uint16_t> spb{11, 11};
        constexpr BitField<uint16_t> sevenbit{12, 12};
        constexpr BitField<uint16_t> msb{13, 13};
        constexpr BitField<uint16_t> par{14, 14};
        constexpr BitField<uint16_t> pen{15, 15};
    }
    namespace ctlw1 {
        constexpr BitField<uint16_t> glit{1, 0};
    }
    namespace mctlw {
        constexpr BitField<uint16_t> os16{0, 0};
        constexpr BitField<uint16_t> brf{7, 4};
        constexpr BitField<uint16_t> brs{15, 8};
    }
    namespace statw {
        constexpr BitField<uint16_t> busy{0, 0};
        constexpr BitField<uint16_t> addr_ucidle{1, 1};
        constexpr BitField<uint16_t> rxerr{2, 2};
        constexpr BitField<uint16_t> brk{3, 3};
        constexpr BitField<uint16_t> pe{4, 4};
        constexpr BitField<uint16_t> oe{5, 5};
        constexpr BitField<uint16_t> fe{6, 6};
        constexpr BitField<uint16_t> listen{7, 7};
    }
    namespace abctl {
        constexpr BitField<uint16_t> abden{0, 0};
        constexpr BitField<uint16_t> btoe{2, 2};
        constexpr BitField<uint16_t> stoe{3, 3};
        constexpr BitField<uint16_t> delim{5, 4};
    }
    namespace irctl {
        constexpr BitField<uint16_t> iren{0, 0};
        constexpr BitField<uint16_t> irtxclk{1, 1};
        constexpr BitField<uint16_t> irtxpl{7, 2};
        constexpr BitField<uint16_t> irrxfe{8, 8};
        constexpr BitField<uint16_t> irrxpl{9, 9};
        constexpr BitField<uint16_t> irrxfl{13, 10};
    }
    namespace ifg {
        constexpr BitField<uint16_t> rxifg{0, 0};
        constexpr BitField<uint16_t> txifg{1, 1};
        constexpr BitField<uint16_t> sttifg{2, 2};
        constexpr BitField<uint16_t> txcptifg{3, 3};
    }
}
