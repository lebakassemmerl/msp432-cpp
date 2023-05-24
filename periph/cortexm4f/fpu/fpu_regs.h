// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <type_traits>
#include <cstddef>

#include "helpers.h"
#include "register.h"

#pragma pack(1)
class FpuRegisters {
public:
    FpuRegisters() = delete;
    FpuRegisters(FpuRegisters&) = delete;
    FpuRegisters(FpuRegisters&&) = delete;
    ~FpuRegisters() = delete;

    Reserved<uint32_t> _reserved0[866];
    ReadWrite<uint32_t> cpacr;
    Reserved<uint32_t> _reserved1[106];
    ReadWrite<uint32_t> fpccr;
    ReadWrite<uint32_t> fpcar;
    ReadWrite<uint32_t> fpdscr;
    ReadOnly<uint32_t> mvfr0;
    ReadOnly<uint32_t> mvfr1;
};
#pragma pack()

static_assert(std::is_standard_layout<FpuRegisters>::value,
    "FpuRegisters isn't standard layout");

constexpr size_t FPU_BASE = 0xE000E000;

namespace fpuregs {
    namespace cpacr {
        constexpr BitField<uint32_t> cp11{23, 22};
        constexpr BitField<uint32_t> cp10{21, 20};
    }

    namespace fpccr {
        constexpr BitField<uint32_t> aspen{31, 31};
        constexpr BitField<uint32_t> lspen{30, 30};
        constexpr BitField<uint32_t> monrdy{8, 8};
        constexpr BitField<uint32_t> bfrdy{6, 6};
        constexpr BitField<uint32_t> mmrdy{5, 5};
        constexpr BitField<uint32_t> hfrdy{4, 4};
        constexpr BitField<uint32_t> thread{3, 3};
        constexpr BitField<uint32_t> user{1, 1};
        constexpr BitField<uint32_t> lspact{0, 0};
    }
    namespace fpcar {
        constexpr BitField<uint32_t> address{30, 2};
    }
    namespace fpdscr {
        constexpr BitField<uint32_t> ahp{26, 26};
        constexpr BitField<uint32_t> dn{25, 25};
        constexpr BitField<uint32_t> fz{24, 24};
        constexpr BitField<uint32_t> rmode{23, 22};
    }
    namespace mvfr0 {
        constexpr BitField<uint32_t> fp_rounding_modes{31, 28};
        constexpr BitField<uint32_t> short_vectors{27, 24};
        constexpr BitField<uint32_t> square_root{23, 20};
        constexpr BitField<uint32_t> divide{19, 16};
        constexpr BitField<uint32_t> fp_exception_trapping{15, 12};
        constexpr BitField<uint32_t> double_precision{11, 8};
        constexpr BitField<uint32_t> single_precision{7, 4};
        constexpr BitField<uint32_t> a_simd_registers{3, 0};
    }
    namespace mvfr1 {
        constexpr BitField<uint32_t> fp_fused_mac{31, 28};
        constexpr BitField<uint32_t> fp_hpfp{27, 24};
        constexpr BitField<uint32_t> d_nan_mode{7, 4};
        constexpr BitField<uint32_t> ftz_mode{3, 0};
    }
}
