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
class PcmRegisters {
public:
    PcmRegisters() = delete;
    PcmRegisters(PcmRegisters&) = delete;
    PcmRegisters(PcmRegisters&&) = delete;
    ~PcmRegisters() = delete;

    ReadWrite<uint32_t> ctl0;
    ReadWrite<uint32_t> ctl1;
    ReadWrite<uint32_t> ie;
    ReadOnly<uint32_t> ifg;
    WriteOnly<uint32_t> clrifg;
};
#pragma pack()

static_assert(std::is_standard_layout<PcmRegisters>::value, "PcmRegisters isn't standard layout");

constexpr size_t PCM_BASE = 0x40010000;

namespace pcmregs {
    constexpr uint32_t KEY = 0x695A;

    namespace ctl0 {
        constexpr BitField<uint32_t> amr(3, 0);
        constexpr BitField<uint32_t> lpmr(7, 4);
        constexpr BitField<uint32_t> cpm(13, 8);
        constexpr BitField<uint32_t> key(31, 16);
    }
    namespace ctl1 {
        constexpr BitField<uint32_t> locklpm5(0, 0);
        constexpr BitField<uint32_t> lockbkup(1, 1);
        constexpr BitField<uint32_t> force_lpm_entry(2, 2);
        constexpr BitField<uint32_t> pmr_busy(8, 8);
        constexpr BitField<uint32_t> key(31, 16);
    }
    namespace ifg {
        constexpr BitField<uint32_t> lpm_invalid_tr_ifg(0, 0);
        constexpr BitField<uint32_t> lpm_invalid_clk_ifg(1, 1);
        constexpr BitField<uint32_t> am_invalid_tr_ifg(2, 2);
        constexpr BitField<uint32_t> dcdc_error_ifg(6, 6);
    }
}
