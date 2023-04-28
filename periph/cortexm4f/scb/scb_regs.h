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
class ScbRegisters {
public:
    ScbRegisters() = delete;
    ScbRegisters(ScbRegisters&) = delete;
    ScbRegisters(ScbRegisters&&) = delete;
    ~ScbRegisters() = delete;

    Reserved<uint32_t> _reserved0[2];
    ReadWrite<uint32_t> actlr;
    Reserved<uint32_t> _reserved1[829];
    ReadOnly<uint32_t> cpuid;
    ReadWrite<uint32_t> icsr;
    ReadWrite<uint32_t> vtor;
    ReadWrite<uint32_t> aircr;
    ReadWrite<uint32_t> scr;
    ReadWrite<uint32_t> ccr;
    ReadWrite<uint32_t> shpr1;
    ReadWrite<uint32_t> shpr2;
    ReadWrite<uint32_t> shpr3;
    ReadWrite<uint32_t> shcsr;
    ReadWrite<uint32_t> cfsr;
    ReadWrite<uint32_t> hfsr;
    ReadWrite<uint32_t> dfsr;
    ReadWrite<uint32_t> mmfar;
    ReadWrite<uint32_t> bfar;
    ReadWrite<uint32_t> afsr;
    ReadOnly<uint32_t> pfr0;
    ReadOnly<uint32_t> pfr1;
    ReadOnly<uint32_t> dfr0;
    ReadOnly<uint32_t> afr0;
    ReadOnly<uint32_t> mmfr0;
    ReadOnly<uint32_t> mmfr1;
    ReadOnly<uint32_t> mmfr2;
    ReadOnly<uint32_t> mmfr3;
    ReadOnly<uint32_t> isar0;
    ReadOnly<uint32_t> isar1;
    ReadOnly<uint32_t> isar2;
    ReadOnly<uint32_t> isar3;
    ReadOnly<uint32_t> isar4;
    Reserved<uint32_t> _reserved9[5];
    ReadWrite<uint32_t> cpacr;
};
#pragma pack()

static_assert(std::is_standard_layout<ScbRegisters>::value,
    "ScbRegisters isn't standard layout");

constexpr size_t SCB_BASE = 0xE000E000;

namespace scbregs {
    namespace actlr {
        constexpr BitField<uint32_t> dismcycint{0, 0};
        constexpr BitField<uint32_t> disdefwbuf{1, 1};
        constexpr BitField<uint32_t> disfold{2, 2};
        constexpr BitField<uint32_t> disfpca{8, 8};
        constexpr BitField<uint32_t> disoofp{9, 9};
    }
    namespace cpuid {
        constexpr BitField<uint32_t> revision{3, 0};
        constexpr BitField<uint32_t> partno{15, 4};
        constexpr BitField<uint32_t> constant{19, 16};
        constexpr BitField<uint32_t> variant{23, 20};
        constexpr BitField<uint32_t> implementer{31, 24};
    }
    namespace icsr {
        constexpr BitField<uint32_t> vectactive{8, 0};
        constexpr BitField<uint32_t> rettobase{11, 11};
        constexpr BitField<uint32_t> vectpending{17, 12};
        constexpr BitField<uint32_t> isrpending{22, 22};
        constexpr BitField<uint32_t> isrpreempt{23, 23};
        constexpr BitField<uint32_t> pendstclr{25, 25};
        constexpr BitField<uint32_t> pendstset{26, 26};
        constexpr BitField<uint32_t> pendsvclr{27, 27};
        constexpr BitField<uint32_t> pendsvset{28, 28};
        constexpr BitField<uint32_t> nmipendset{31, 31};
    }
    namespace vtor {
        constexpr BitField<uint32_t> tbloff{28, 7};
        constexpr BitField<uint32_t> tblbase{29, 29};
    }
    namespace aircr {
        constexpr BitField<uint32_t> vectreset{0, 0};
        constexpr BitField<uint32_t> vectclractive{1, 1};
        constexpr BitField<uint32_t> sysresetreq{2, 2};
        constexpr BitField<uint32_t> prigroup{10, 8};
        constexpr BitField<uint32_t> endianess{15, 15};
        constexpr BitField<uint32_t> vectkey{31, 16};
    }
    namespace scr {
        constexpr BitField<uint32_t> sleeponexit{1, 1};
        constexpr BitField<uint32_t> sleepdeep{2, 2};
        constexpr BitField<uint32_t> sevonpend{4, 4};
    }
    namespace ccr {
        constexpr BitField<uint32_t> nonbasethredena{0, 0};
        constexpr BitField<uint32_t> usersetmpend{1, 1};
        constexpr BitField<uint32_t> unalign_trp{3, 3};
        constexpr BitField<uint32_t> div_0_trp{4, 4};
        constexpr BitField<uint32_t> bfhfnmign{8, 8};
        constexpr BitField<uint32_t> stkalign{9, 9};
    }
    namespace shpr1 {
        constexpr BitField<uint32_t> pri_4{7, 0};
        constexpr BitField<uint32_t> pri_5{15, 8};
        constexpr BitField<uint32_t> pri_6{23, 16};
        constexpr BitField<uint32_t> pri_7{31, 24};
    }
    namespace shcsr {
        constexpr BitField<uint32_t> memfaultact{0, 0};
        constexpr BitField<uint32_t> busfaultact{1, 1};
        constexpr BitField<uint32_t> usgfaultact{3, 3};
        constexpr BitField<uint32_t> svcallact{7, 7};
        constexpr BitField<uint32_t> monitoract{8, 8};
        constexpr BitField<uint32_t> pendsvact{10, 10};
        constexpr BitField<uint32_t> systickact{11, 11};
        constexpr BitField<uint32_t> usgfaultpended{12, 12};
        constexpr BitField<uint32_t> memfaultpended{13, 13};
        constexpr BitField<uint32_t> busfaultpended{14, 14};
        constexpr BitField<uint32_t> svcallpended{15, 15};
        constexpr BitField<uint32_t> memfaultena{16, 16};
        constexpr BitField<uint32_t> busfaultena{17, 17};
        constexpr BitField<uint32_t> usgfaultena{18, 18};
    }
    namespace cfsr {
        constexpr BitField<uint32_t> iaccviol{0, 0};
        constexpr BitField<uint32_t> daccviol{1, 1};
        constexpr BitField<uint32_t> munstkerr{3, 3};
        constexpr BitField<uint32_t> mstkerr{4, 4};
        constexpr BitField<uint32_t> mmarvalid{7, 7};
        constexpr BitField<uint32_t> ibuserr{8, 8};
        constexpr BitField<uint32_t> preciserr{9, 9};
        constexpr BitField<uint32_t> impreciserr{10, 10};
        constexpr BitField<uint32_t> unstkerr{11, 11};
        constexpr BitField<uint32_t> stkerr{12, 12};
        constexpr BitField<uint32_t> bfarvalid{15, 15};
        constexpr BitField<uint32_t> undefinstr{16, 16};
        constexpr BitField<uint32_t> invstate{17, 17};
        constexpr BitField<uint32_t> invpc{18, 18};
        constexpr BitField<uint32_t> nocp{19, 19};
        constexpr BitField<uint32_t> unaligned{24, 24};
        constexpr BitField<uint32_t> divbyzero{25, 25};
        constexpr BitField<uint32_t> mlsperr{5, 5};
        constexpr BitField<uint32_t> lsperr{13, 13};
    }
    namespace hfsr {
        constexpr BitField<uint32_t> vecttbl{1, 1};
        constexpr BitField<uint32_t> forced{30, 30};
        constexpr BitField<uint32_t> debugevt{31, 31};
    }
    namespace dfsr {
        constexpr BitField<uint32_t> halted{0, 0};
        constexpr BitField<uint32_t> bkpt{1, 1};
        constexpr BitField<uint32_t> dwttrap{2, 2};
        constexpr BitField<uint32_t> vcatch{3, 3};
        constexpr BitField<uint32_t> external{4, 4};
    }
    namespace pfr0 {
        constexpr BitField<uint32_t> state0{3, 0};
        constexpr BitField<uint32_t> state1{7, 4};
    }
    namespace pfr1 {
        constexpr BitField<uint32_t> microcontroller_programmers_model{11, 8};
    }
    namespace dfr0 {
        constexpr BitField<uint32_t> microcontroller_debug_model{23, 20};
    }
    namespace mmfr0 {
        constexpr BitField<uint32_t> pmsa_support{7, 4};
        constexpr BitField<uint32_t> cache_coherence_support{11, 8};
        constexpr BitField<uint32_t> outer_non_sharable_support{15, 12};
        constexpr BitField<uint32_t> auxiliary_register_support{23, 20};
    }
    namespace mmfr2 {
        constexpr BitField<uint32_t> wait_for_interrupt_stalling{27, 24};
    }
    namespace isar0 {
        constexpr BitField<uint32_t> bitcount_instrs{7, 4};
        constexpr BitField<uint32_t> bitfield_instrs{11, 8};
        constexpr BitField<uint32_t> cmpbranch_instrs{15, 12};
        constexpr BitField<uint32_t> coproc_instrs{19, 16};
        constexpr BitField<uint32_t> debug_instrs{23, 20};
        constexpr BitField<uint32_t> divide_instrs{27, 24};
    }
    namespace isar1 {
        constexpr BitField<uint32_t> extend_insrs{15, 12};
        constexpr BitField<uint32_t> ifthen_instrs{19, 16};
        constexpr BitField<uint32_t> immediate_instrs{23, 20};
        constexpr BitField<uint32_t> interwork_instrs{27, 24};
    }
    namespace isar2 {
        constexpr BitField<uint32_t> loadstore_instrs{3, 0};
        constexpr BitField<uint32_t> memhint_instrs{7, 4};
        constexpr BitField<uint32_t> multiaccessint_instrs{11, 8};
        constexpr BitField<uint32_t> mult_instrs{15, 12};
        constexpr BitField<uint32_t> mults_instrs{19, 16};
        constexpr BitField<uint32_t> multu_instrs{23, 20};
        constexpr BitField<uint32_t> reversal_instrs{31, 28};
    }
    namespace isar3 {
        constexpr BitField<uint32_t> satruate_instrs{3, 0};
        constexpr BitField<uint32_t> simd_instrs{7, 4};
        constexpr BitField<uint32_t> svc_instrs{11, 8};
        constexpr BitField<uint32_t> syncprim_instrs{15, 12};
        constexpr BitField<uint32_t> tabbranch_instrs{19, 16};
        constexpr BitField<uint32_t> thumbcopy_instrs{23, 20};
        constexpr BitField<uint32_t> truenop_instrs{27, 24};
    }
    namespace isar4 {
        constexpr BitField<uint32_t> unpriv_instrs{3, 0};
        constexpr BitField<uint32_t> withshifts_instrs{7, 4};
        constexpr BitField<uint32_t> writeback_instrs{11, 8};
        constexpr BitField<uint32_t> barrier_instrs{19, 16};
        constexpr BitField<uint32_t> syncprim_instrs_frac{23, 20};
        constexpr BitField<uint32_t> psr_m_instrs{27, 24};
    }
    namespace cpacr {
        constexpr BitField<uint32_t> cp11{23, 22};
        constexpr BitField<uint32_t> cp10{21, 20};
    }
}
