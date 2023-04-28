// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <type_traits>
#include <cstddef>
#include <cstdint>

#include "helpers.h"
#include "register.h"

#pragma pack(1)
class CsRegisters {
public:
    CsRegisters() = delete;
    CsRegisters(CsRegisters&) = delete;
    CsRegisters(CsRegisters&&) = delete;
    ~CsRegisters() = delete;

    ReadWrite<uint32_t> key;
    ReadWrite<uint32_t> ctl0;
    ReadWrite<uint32_t> ctl1;
    ReadWrite<uint32_t> ctl2;
    ReadWrite<uint32_t> ctl3;
    Reserved<uint32_t> _reserved0[7];
    ReadWrite<uint32_t> clken;
    ReadOnly<uint32_t> stat;
    Reserved<uint32_t> _reserved1[2];
    ReadWrite<uint32_t> ie;
    Reserved<uint32_t> _reserved2[1];
    ReadOnly<uint32_t> ifg;
    Reserved<uint32_t> _reserved3[1];
    WriteOnly<uint32_t> clrifg;
    Reserved<uint32_t> _reserved4[1];
    WriteOnly<uint32_t> setifg;
    Reserved<uint32_t> _reserved5[1];
    ReadWrite<uint32_t> dcoercal0;
    ReadWrite<uint32_t> dcoercal1;
};
#pragma pack()

static_assert(std::is_standard_layout<CsRegisters>::value, "CsRegisters isn't standard layout");

constexpr size_t CS_BASE = 0x40010400;

namespace csregs {
    namespace key {
        constexpr uint32_t KEY = 0x695A;
        constexpr BitField<uint32_t> key{15, 0};
    }
    namespace ctl0 {
        constexpr BitField<uint32_t> dcotune(9, 0);
        constexpr BitField<uint32_t> dcorsel(18, 16);
        constexpr BitField<uint32_t> dcores(22, 22);
        constexpr BitField<uint32_t> dcoen(23, 23);
    }
    namespace ctl1 {
        constexpr BitField<uint32_t> selm(2, 0);
        constexpr BitField<uint32_t> sels(6, 4);
        constexpr BitField<uint32_t> sela(10, 8);
        constexpr BitField<uint32_t> selb(12, 12);
        constexpr BitField<uint32_t> divm(18, 16);
        constexpr BitField<uint32_t> divhs(22, 20);
        constexpr BitField<uint32_t> diva(26, 24);
        constexpr BitField<uint32_t> divs(30, 28);
    }
    namespace ctl2 {
        constexpr BitField<uint32_t> lfxtdrive(1, 0);
        constexpr BitField<uint32_t> lfxtagcoff(7, 7);
        constexpr BitField<uint32_t> lfxt_en(8, 8);
        constexpr BitField<uint32_t> lfxtbypass(9, 9);
        constexpr BitField<uint32_t> hfxtdrive(16, 16);
        constexpr BitField<uint32_t> hfxtfreq(22, 20);
        constexpr BitField<uint32_t> hfxt_en(24, 24);
        constexpr BitField<uint32_t> hfxtbypass(25, 25);
    }
    namespace ctl3 {
        constexpr BitField<uint32_t> fcntlf(1, 0);
        constexpr BitField<uint32_t> rfcntlf(2, 2);
        constexpr BitField<uint32_t> fcntlf_en(3, 3);
        constexpr BitField<uint32_t> fcnthf(5, 4);
        constexpr BitField<uint32_t> rfcnthf(6, 6);
        constexpr BitField<uint32_t> fcnthf_en(7, 7);
        constexpr BitField<uint32_t> fcnthf2(9, 8);
        constexpr BitField<uint32_t> rfcnthf2(10, 10);
        constexpr BitField<uint32_t> fcnthf2_en(11, 11);
    }
    namespace clken {
        constexpr BitField<uint32_t> aclk_en(0, 0);
        constexpr BitField<uint32_t> mclk_en(1, 1);
        constexpr BitField<uint32_t> hsmclk_en(2, 2);
        constexpr BitField<uint32_t> smclk_en(3, 3);
        constexpr BitField<uint32_t> vlo_en(8, 8);
        constexpr BitField<uint32_t> refo_en(9, 9);
        constexpr BitField<uint32_t> modosc_en(10, 10);
        constexpr BitField<uint32_t> refofsel(15, 15);
    }
    namespace stat {
        constexpr BitField<uint32_t> dco_on(0, 0);
        constexpr BitField<uint32_t> dcobias_on(1, 1);
        constexpr BitField<uint32_t> hfxt_on(2, 2);
        constexpr BitField<uint32_t> hfxt2_on(3, 3);
        constexpr BitField<uint32_t> modosc_on(4, 4);
        constexpr BitField<uint32_t> vlo_on(5, 5);
        constexpr BitField<uint32_t> lfxt_on(6, 6);
        constexpr BitField<uint32_t> refo_on(7, 7);
        constexpr BitField<uint32_t> aclk_on(16, 16);
        constexpr BitField<uint32_t> mclk_on(17, 17);
        constexpr BitField<uint32_t> hsmclk_on(18, 18);
        constexpr BitField<uint32_t> smclk_on(19, 19);
        constexpr BitField<uint32_t> modclk_on(20, 20);
        constexpr BitField<uint32_t> vloclk_on(21, 21);
        constexpr BitField<uint32_t> lfxtclk_on(22, 22);
        constexpr BitField<uint32_t> refoclk_on(23, 23);
        constexpr BitField<uint32_t> aclk_ready(24, 24);
        constexpr BitField<uint32_t> mclk_ready(25, 25);
        constexpr BitField<uint32_t> hsmclk_ready(26, 26);
        constexpr BitField<uint32_t> smclk_ready(27, 27);
        constexpr BitField<uint32_t> bclk_ready(28, 28);
    }
    namespace ifg {
        constexpr BitField<uint32_t> lfxt(0, 0);
        constexpr BitField<uint32_t> hfxt(1, 1);
        constexpr BitField<uint32_t> hfxt2(2, 2);
        constexpr BitField<uint32_t> dcor_sht(5, 5);
        constexpr BitField<uint32_t> dcor_opn(6, 6);
        constexpr BitField<uint32_t> fcntlf(8, 8);
        constexpr BitField<uint32_t> fcnthf(9, 9);
        constexpr BitField<uint32_t> fcnthf2(11, 11);
        constexpr BitField<uint32_t> pllool(12, 12);
        constexpr BitField<uint32_t> plllos(13, 13);
        constexpr BitField<uint32_t> plloor(14, 14);
        constexpr BitField<uint32_t> cal(15, 15);
    }
    namespace dcoercal0 {
        constexpr BitField<uint32_t> dco_tccal(1, 0);
        constexpr BitField<uint32_t> dco_fcal_rsel04(25, 16);
    }
    namespace dcoercal1 {
        constexpr BitField<uint32_t> dco_fcal_rsel5(9, 0);
    }
}
