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
class SysCtlRegisters {
public:
    SysCtlRegisters() = delete;
    SysCtlRegisters(SysCtlRegisters&) = delete;
    SysCtlRegisters(SysCtlRegisters&&) = delete;
    ~SysCtlRegisters() = delete;

    ReadWrite<uint32_t> reboot_ctl;
    ReadWrite<uint32_t> nmi_ctlstat;
    ReadWrite<uint32_t> wdtreset_ctl;
    ReadWrite<uint32_t> perihalt_ctl;
    ReadOnly<uint32_t> sram_size;
    ReadWrite<uint32_t> sram_banken;
    ReadWrite<uint32_t> sram_bankret;
    Reserved<uint32_t> _reserved0[1];
    ReadOnly<uint32_t> flash_size;
    Reserved<uint32_t> _reserved1[3];
    ReadWrite<uint32_t> dio_gltflt_ctl;
    Reserved<uint32_t> _reserved2[3];
    ReadWrite<uint32_t> secdata_unlock;
    Reserved<uint32_t> _reserved3[1007];
    ReadWrite<uint32_t> master_unlock;
    ReadWrite<uint32_t> bootover_reqx[2];
    ReadWrite<uint32_t> bootover_ack;
    ReadWrite<uint32_t> reset_req;
    ReadWrite<uint32_t> reset_statover;
    Reserved<uint32_t> _reserved4[2];
    ReadOnly<uint32_t> system_stat;
};
#pragma pack()

static_assert(std::is_standard_layout<SysCtlRegisters>::value,
    "SysctlRegisters isn't standard layout");

constexpr size_t SYSCTL_BASE = 0xE0043000;

namespace sysctlregs {
    namespace reboot_ctl {
        constexpr BitField<uint32_t> reboot(0, 0);
        constexpr BitField<uint32_t> wkey(15, 8);
    }
    namespace nmi_ctlstat {
        constexpr BitField<uint32_t> cs_src(0, 0);
        constexpr BitField<uint32_t> pss_src(1, 1);
        constexpr BitField<uint32_t> pcm_src(2, 2);
        constexpr BitField<uint32_t> pin_src(3, 3);
        constexpr BitField<uint32_t> cs_flg(16, 16);
        constexpr BitField<uint32_t> pss_flg(17, 17);
        constexpr BitField<uint32_t> pcm_flg(18, 18);
        constexpr BitField<uint32_t> pin_flg(19, 19);
    }
    namespace wdtreset_ctl {
        constexpr BitField<uint32_t> timeout(0, 0);
        constexpr BitField<uint32_t> violation(1, 1);
    }
    namespace perihalt_ctl {
        constexpr BitField<uint32_t> halt_t16_0(0, 0);
        constexpr BitField<uint32_t> halt_t16_1(1, 1);
        constexpr BitField<uint32_t> halt_t16_2(2, 2);
        constexpr BitField<uint32_t> halt_t16_3(3, 3);
        constexpr BitField<uint32_t> halt_t32_0(4, 4);
        constexpr BitField<uint32_t> halt_eua0(5, 5);
        constexpr BitField<uint32_t> halt_eua1(6, 6);
        constexpr BitField<uint32_t> halt_eua2(7, 7);
        constexpr BitField<uint32_t> halt_eua3(8, 8);
        constexpr BitField<uint32_t> halt_eub0(9, 9);
        constexpr BitField<uint32_t> halt_eub1(10, 10);
        constexpr BitField<uint32_t> halt_eub2(11, 11);
        constexpr BitField<uint32_t> halt_eub3(12, 12);
        constexpr BitField<uint32_t> halt_adc(13, 13);
        constexpr BitField<uint32_t> halt_wdt(14, 14);
        constexpr BitField<uint32_t> halt_dma(15, 15);
    }
    namespace sram_size {
        constexpr BitField<uint32_t> size(31, 0);
    }
    namespace sram_banken {
        constexpr BitField<uint32_t> bnk0_en(0, 0);
        constexpr BitField<uint32_t> bnk1_en(1, 1);
        constexpr BitField<uint32_t> bnk2_en(2, 2);
        constexpr BitField<uint32_t> bnk3_en(3, 3);
        constexpr BitField<uint32_t> bnk4_en(4, 4);
        constexpr BitField<uint32_t> bnk5_en(5, 5);
        constexpr BitField<uint32_t> bnk6_en(6, 6);
        constexpr BitField<uint32_t> bnk7_en(7, 7);
        constexpr BitField<uint32_t> sram_rdy(16, 16);
    }
    namespace sram_bankret {
        constexpr BitField<uint32_t> bnk0_ret(0, 0);
        constexpr BitField<uint32_t> bnk1_ret(1, 1);
        constexpr BitField<uint32_t> bnk2_ret(2, 2);
        constexpr BitField<uint32_t> bnk3_ret(3, 3);
        constexpr BitField<uint32_t> bnk4_ret(4, 4);
        constexpr BitField<uint32_t> bnk5_ret(5, 5);
        constexpr BitField<uint32_t> bnk6_ret(6, 6);
        constexpr BitField<uint32_t> bnk7_ret(7, 7);
        constexpr BitField<uint32_t> sram_rdy(16, 16);
    }
    namespace dio_gltflt_ctl {
        constexpr BitField<uint32_t> gltch_en(0, 0);
    }
    namespace secdata_unlock {
        constexpr BitField<uint32_t> unlkey(15, 0);
    }
    namespace master_unlock {
        constexpr BitField<uint32_t> unlkey(15, 0);
    }
    namespace reset_req {
        constexpr BitField<uint32_t> por(0, 0);
        constexpr BitField<uint32_t> reboot(1, 1);
        constexpr BitField<uint32_t> wkey(15, 8);
    }
    namespace reset_statover {
        constexpr BitField<uint32_t> soft(0, 0);
        constexpr BitField<uint32_t> hard(1, 1);
        constexpr BitField<uint32_t> reboot(2, 2);
        constexpr BitField<uint32_t> soft_over(8, 8);
        constexpr BitField<uint32_t> hard_over(9, 9);
        constexpr BitField<uint32_t> rbt_over(10, 10);
    }
    namespace system_stat {
        constexpr BitField<uint32_t> dbg_sec_act(3, 3);
        constexpr BitField<uint32_t> jtag_swd_lock_act(4, 4);
        constexpr BitField<uint32_t> ip_prot_act(5, 5);
    }
}
