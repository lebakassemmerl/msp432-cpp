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
class UsciBRegisters {
public:
    UsciBRegisters() = delete;
    UsciBRegisters(UsciBRegisters&) = delete;
    UsciBRegisters(UsciBRegisters&&) = delete;
    ~UsciBRegisters() = delete;

    ReadWrite<uint16_t> ctlw0;
    ReadWrite<uint16_t> ctlw1;
    Reserved<uint16_t> _reserved0[1];
    ReadWrite<uint16_t> brw;
    ReadWrite<uint16_t> statw;
    ReadWrite<uint16_t> tbcnt;
    ReadOnly<uint16_t> rxbuf;
    ReadWrite<uint16_t> txbuf;
    Reserved<uint16_t> _reserved1[2];
    ReadWrite<uint16_t> i2coa0;
    ReadWrite<uint16_t> i2coa1;
    ReadWrite<uint16_t> i2coa2;
    ReadWrite<uint16_t> i2coa3;
    ReadOnly<uint16_t> addrx;
    ReadWrite<uint16_t> addmask;
    ReadWrite<uint16_t> i2csa;
    Reserved<uint16_t> _reserved2[4];
    ReadWrite<uint16_t> ie;
    ReadWrite<uint16_t> ifg;
    ReadOnly<uint16_t> iv;
};
#pragma pack()

static_assert(std::is_standard_layout<UsciBRegisters>::value,
    "UsciBRegisters isn't standard layout");

constexpr size_t USCIB0_BASE = 0x40002000;
constexpr size_t USCIB1_BASE = 0x40002400;
constexpr size_t USCIB2_BASE = 0x40002800;
constexpr size_t USCIB3_BASE = 0x40002C00;

namespace uscibregs {
    namespace ctlw0 {
        constexpr BitField<uint16_t> swrst{0, 0};
        constexpr BitField<uint16_t> txstt{1, 1};
        constexpr BitField<uint16_t> txstp{2, 2};
        constexpr BitField<uint16_t> txnack{3, 3};
        constexpr BitField<uint16_t> tr{4, 4};
        constexpr BitField<uint16_t> txack{5, 5};
        constexpr BitField<uint16_t> ssel{7, 6};
        constexpr BitField<uint16_t> sync{8, 8};
        constexpr BitField<uint16_t> mode{10, 9};
        constexpr BitField<uint16_t> mst{11, 11};
        constexpr BitField<uint16_t> mm{13, 13};
        constexpr BitField<uint16_t> sla10{14, 14};
        constexpr BitField<uint16_t> a10{15, 15};
    }
    namespace ctlw1 {
        constexpr BitField<uint16_t> glit{1, 0};
        constexpr BitField<uint16_t> astp{3, 2};
        constexpr BitField<uint16_t> swack{4, 4};
        constexpr BitField<uint16_t> stpnack{5, 5};
        constexpr BitField<uint16_t> clto{7, 6};
        constexpr BitField<uint16_t> etxint{8, 8};
    }
    namespace statw {
        constexpr BitField<uint16_t> bbusy{4, 4};
        constexpr BitField<uint16_t> gc{5, 5};
        constexpr BitField<uint16_t> scllow{6, 6};
        constexpr BitField<uint16_t> bcnt{15, 8};
    }
    namespace i2coa0 {
        constexpr BitField<uint16_t> i2coa0{9, 0};
        constexpr BitField<uint16_t> oaen{10, 10};
        constexpr BitField<uint16_t> gcen{15, 15};
    }
    namespace i2coax {
        constexpr BitField<uint16_t> i2coax{9, 0};
        constexpr BitField<uint16_t> oaen{10, 10};
    }
    namespace ifg {
        constexpr BitField<uint16_t> rxifg0{0, 0};
        constexpr BitField<uint16_t> txifg0{1, 1};
        constexpr BitField<uint16_t> sttifg{2, 2};
        constexpr BitField<uint16_t> stpifg{3, 3};
        constexpr BitField<uint16_t> alifg{4, 4};
        constexpr BitField<uint16_t> nackifg{5, 5};
        constexpr BitField<uint16_t> bcntifg{6, 6};
        constexpr BitField<uint16_t> cltoifg{7, 7};
        constexpr BitField<uint16_t> rxifg1{8, 8};
        constexpr BitField<uint16_t> txifg1{9, 9};
        constexpr BitField<uint16_t> rxifg2{10, 10};
        constexpr BitField<uint16_t> txifg2{11, 11};
        constexpr BitField<uint16_t> rxifg3{12, 12};
        constexpr BitField<uint16_t> txifg3{13, 13};
        constexpr BitField<uint16_t> bit9ifg{14, 14};
    }
}
