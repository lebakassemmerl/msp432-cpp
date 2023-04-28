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
class FlCtlRegisters {
public:
    FlCtlRegisters() = delete;
    FlCtlRegisters(FlCtlRegisters&) = delete;
    FlCtlRegisters(FlCtlRegisters&&) = delete;
    ~FlCtlRegisters() = delete;

    ReadOnly<uint32_t> power_stat;
    Reserved<uint32_t> _reserved0[3];
    ReadWrite<uint32_t> bank0_rdctl;
    ReadWrite<uint32_t> bank1_rdctl;
    Reserved<uint32_t> _reserved1[2];
    ReadWrite<uint32_t> rdbrst_ctlstat;
    ReadWrite<uint32_t> rdbrst_startaddr;
    ReadWrite<uint32_t> rdbrst_len;
    Reserved<uint32_t> _reserved2[4];
    ReadWrite<uint32_t> rdbrst_failaddr;
    ReadWrite<uint32_t> rdbrst_failcnt;
    Reserved<uint32_t> _reserved3[3];
    ReadWrite<uint32_t> prg_ctlstat;
    ReadWrite<uint32_t> prgbrst_ctlstat;
    ReadWrite<uint32_t> prgbrst_startaddr;
    Reserved<uint32_t> _reserved4[1];
    ReadWrite<uint32_t> prgbrst_data0_0;
    ReadWrite<uint32_t> prgbrst_data0_1;
    ReadWrite<uint32_t> prgbrst_data0_2;
    ReadWrite<uint32_t> prgbrst_data0_3;
    ReadWrite<uint32_t> prgbrst_data1_0;
    ReadWrite<uint32_t> prgbrst_data1_1;
    ReadWrite<uint32_t> prgbrst_data1_2;
    ReadWrite<uint32_t> prgbrst_data1_3;
    ReadWrite<uint32_t> prgbrst_data2_0;
    ReadWrite<uint32_t> prgbrst_data2_1;
    ReadWrite<uint32_t> prgbrst_data2_2;
    ReadWrite<uint32_t> prgbrst_data2_3;
    ReadWrite<uint32_t> prgbrst_data3_0;
    ReadWrite<uint32_t> prgbrst_data3_1;
    ReadWrite<uint32_t> prgbrst_data3_2;
    ReadWrite<uint32_t> prgbrst_data3_3;
    ReadWrite<uint32_t> erase_ctlstat;
    ReadWrite<uint32_t> erase_sectaddr;
    Reserved<uint32_t> _reserved5[2];
    ReadWrite<uint32_t> bank0_info_weprot;
    ReadWrite<uint32_t> bank0_main_weprot;
    Reserved<uint32_t> _reserved6[2];
    ReadWrite<uint32_t> bank1_info_weprot;
    ReadWrite<uint32_t> bank1_main_weprot;
    Reserved<uint32_t> _reserved7[2];
    ReadWrite<uint32_t> bmrk_ctlstat;
    ReadWrite<uint32_t> bmrk_ifetch;
    ReadWrite<uint32_t> bmrk_dread;
    ReadWrite<uint32_t> bmrk_cmp;
    Reserved<uint32_t> _reserved8[4];
    ReadWrite<uint32_t> ifg;
    ReadWrite<uint32_t> ie;
    ReadWrite<uint32_t> clrifg;
    ReadWrite<uint32_t> setifg;
    ReadOnly<uint32_t> read_timctl;
    ReadOnly<uint32_t> readmargin_timctl;
    ReadOnly<uint32_t> prgver_timctl;
    ReadOnly<uint32_t> ersver_timctl;
    ReadOnly<uint32_t> lkgver_timctl;
    ReadOnly<uint32_t> program_timctl;
    ReadOnly<uint32_t> erase_timctl;
    ReadOnly<uint32_t> masserase_timctl;
    ReadOnly<uint32_t> burstprg_timctl;
};
#pragma pack()

static_assert(std::is_standard_layout<FlCtlRegisters>::value,
    "FlctlRegisters isn't standard layout");

constexpr size_t FLCTL_BASE = 0x40011000;

namespace flctlregs {
    namespace power_stat {
        constexpr BitField<uint32_t> pstat(2, 0);
        constexpr BitField<uint32_t> ldostat(3, 3);
        constexpr BitField<uint32_t> vrefstat(4, 4);
        constexpr BitField<uint32_t> irefstat(5, 5);
        constexpr BitField<uint32_t> trimstat(6, 6);
        constexpr BitField<uint32_t> rd_2t(7, 7);
    }
    namespace bank0_rdctl {
        constexpr BitField<uint32_t> rd_mode(3, 0);
        constexpr BitField<uint32_t> bufi(4, 4);
        constexpr BitField<uint32_t> bufd(5, 5);
        constexpr BitField<uint32_t> wait(15, 12);
        constexpr BitField<uint32_t> rd_mode_status(19, 16);
    }
    namespace bank1_rdctl {
        constexpr BitField<uint32_t> rd_mode(3, 0);
        constexpr BitField<uint32_t> bufi(4, 4);
        constexpr BitField<uint32_t> bufd(5, 5);
        constexpr BitField<uint32_t> rd_mode_status(19, 16);
        constexpr BitField<uint32_t> wait(15, 12);
    }
    namespace rdbrst_ctlstat {
        constexpr BitField<uint32_t> start(0, 0);
        constexpr BitField<uint32_t> mem_type(2, 1);
        constexpr BitField<uint32_t> stop_fail(3, 3);
        constexpr BitField<uint32_t> data_cmp(4, 4);
        constexpr BitField<uint32_t> test_en(6, 6);
        constexpr BitField<uint32_t> brst_stat(17, 16);
        constexpr BitField<uint32_t> cmp_err(18, 18);
        constexpr BitField<uint32_t> addr_err(19, 19);
        constexpr BitField<uint32_t> clr_stat(23, 23);
    }
    namespace rdbrst_startaddr {
        constexpr BitField<uint32_t> start_address(20, 0);
    }
    namespace rdbrst_len {
        constexpr BitField<uint32_t> burst_length(20, 0);
    }
    namespace rdbrst_failaddr {
        constexpr BitField<uint32_t> fail_address(20, 0);
    }
    namespace rdbrst_failcnt {
        constexpr BitField<uint32_t> fail_count(16, 0);
    }
    namespace prg_ctlstat {
        constexpr BitField<uint32_t> enable(0, 0);
        constexpr BitField<uint32_t> mode(1, 1);
        constexpr BitField<uint32_t> ver_pre(2, 2);
        constexpr BitField<uint32_t> ver_pst(3, 3);
        constexpr BitField<uint32_t> status(17, 16);
        constexpr BitField<uint32_t> bnk_act(18, 18);
    }
    namespace prgbrst_ctlstat {
        constexpr BitField<uint32_t> start(0, 0);
        constexpr BitField<uint32_t> type(2, 1);
        constexpr BitField<uint32_t> len(5, 3);
        constexpr BitField<uint32_t> auto_pre(6, 6);
        constexpr BitField<uint32_t> auto_pst(7, 7);
        constexpr BitField<uint32_t> burst_status(18, 16);
        constexpr BitField<uint32_t> pre_err(19, 19);
        constexpr BitField<uint32_t> pst_err(20, 20);
        constexpr BitField<uint32_t> addr_err(21, 21);
        constexpr BitField<uint32_t> clr_stat(23, 23);
    }
    namespace prgbrst_startaddr {
        constexpr BitField<uint32_t> start_address(21, 0);
    }
    namespace erase_ctlstat {
        constexpr BitField<uint32_t> start(0, 0);
        constexpr BitField<uint32_t> mode(1, 1);
        constexpr BitField<uint32_t> type(3, 2);
        constexpr BitField<uint32_t> status(17, 16);
        constexpr BitField<uint32_t> addr_err(18, 18);
        constexpr BitField<uint32_t> clr_stat(19, 19);
    }
    namespace erase_sectaddr {
        constexpr BitField<uint32_t> sect_address(21, 0);
    }
    namespace bank_info_weprot {
        constexpr BitField<uint32_t> prot0(0, 0);
        constexpr BitField<uint32_t> prot1(1, 1);
    }
    namespace bank_weprot {
        constexpr BitField<uint32_t> prot0(0, 0);
        constexpr BitField<uint32_t> prot1(1, 1);
        constexpr BitField<uint32_t> prot2(2, 2);
        constexpr BitField<uint32_t> prot3(3, 3);
        constexpr BitField<uint32_t> prot4(4, 4);
        constexpr BitField<uint32_t> prot5(5, 5);
        constexpr BitField<uint32_t> prot6(6, 6);
        constexpr BitField<uint32_t> prot7(7, 7);
        constexpr BitField<uint32_t> prot8(8, 8);
        constexpr BitField<uint32_t> prot9(9, 9);
        constexpr BitField<uint32_t> prot10(10, 10);
        constexpr BitField<uint32_t> prot11(11, 11);
        constexpr BitField<uint32_t> prot12(12, 12);
        constexpr BitField<uint32_t> prot13(13, 13);
        constexpr BitField<uint32_t> prot14(14, 14);
        constexpr BitField<uint32_t> prot15(15, 15);
        constexpr BitField<uint32_t> prot16(16, 16);
        constexpr BitField<uint32_t> prot17(17, 17);
        constexpr BitField<uint32_t> prot18(18, 18);
        constexpr BitField<uint32_t> prot19(19, 19);
        constexpr BitField<uint32_t> prot20(20, 20);
        constexpr BitField<uint32_t> prot21(21, 21);
        constexpr BitField<uint32_t> prot22(22, 22);
        constexpr BitField<uint32_t> prot23(23, 23);
        constexpr BitField<uint32_t> prot24(24, 24);
        constexpr BitField<uint32_t> prot25(25, 25);
        constexpr BitField<uint32_t> prot26(26, 26);
        constexpr BitField<uint32_t> prot27(27, 27);
        constexpr BitField<uint32_t> prot28(28, 28);
        constexpr BitField<uint32_t> prot29(29, 29);
        constexpr BitField<uint32_t> prot30(30, 30);
        constexpr BitField<uint32_t> prot31(31, 31);
    }
    namespace bmrk_ctlstat {
        constexpr BitField<uint32_t> i_bmrk(0, 0);
        constexpr BitField<uint32_t> d_bmrk(1, 1);
        constexpr BitField<uint32_t> cmp_en(2, 2);
        constexpr BitField<uint32_t> cmp_sel(3, 3);
    }
    namespace ifg {
        constexpr BitField<uint32_t> rdbrst(0, 0);
        constexpr BitField<uint32_t> avpre(1, 1);
        constexpr BitField<uint32_t> avpst(2, 2);
        constexpr BitField<uint32_t> prg(3, 3);
        constexpr BitField<uint32_t> prgb(4, 4);
        constexpr BitField<uint32_t> erase(5, 5);
        constexpr BitField<uint32_t> bmrk(8, 8);
        constexpr BitField<uint32_t> prg_err(9, 9);
    }
    namespace read_timctl {
        constexpr BitField<uint32_t> setup(7, 0);
        constexpr BitField<uint32_t> iref_boost1(15, 12);
        constexpr BitField<uint32_t> setup_long(23, 16);
    }
    namespace readmargin_timctl {
        constexpr BitField<uint32_t> setup(7, 0);
    }
    namespace prgver_timctl {
        constexpr BitField<uint32_t> setup(7, 0);
        constexpr BitField<uint32_t> active(11, 8);
        constexpr BitField<uint32_t> hold(15, 12);
    }
    namespace ersver_timctl {
        constexpr BitField<uint32_t> setup(7, 0);
    }
    namespace lkgver_timctl {
        constexpr BitField<uint32_t> setup(7, 0);
    }
    namespace program_timctl {
        constexpr BitField<uint32_t> setup(7, 0);
        constexpr BitField<uint32_t> active(27, 8);
        constexpr BitField<uint32_t> hold(31, 28);
    }
    namespace erase_timctl {
        constexpr BitField<uint32_t> setup(7, 0);
        constexpr BitField<uint32_t> active(27, 8);
        constexpr BitField<uint32_t> hold(31, 28);
    }
    namespace masserase_timctl {
        constexpr BitField<uint32_t> boost_active(7, 0);
        constexpr BitField<uint32_t> boost_hold(15, 8);
    }
    namespace burstprg_timctl {
        constexpr BitField<uint32_t> active(27, 8);
    }
}
