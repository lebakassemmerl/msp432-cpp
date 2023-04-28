// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <array>
#include <type_traits>
#include <cstddef>

#include "helpers.h"
#include "register.h"

#pragma pack(1)
class DmaRegisters {
public:
    DmaRegisters() = delete;
    DmaRegisters(DmaRegisters&) = delete;
    DmaRegisters(DmaRegisters&&) = delete;
    ~DmaRegisters() = delete;

    ReadOnly<uint32_t> device_cfg;
    ReadWrite<uint32_t> sw_chtrig;
    Reserved<uint32_t> _reserved0[2];
    ReadWrite<uint32_t> chx_srccfg[32];
    Reserved<uint32_t> _reserved1[28];
    ReadWrite<uint32_t> int1_srccfg;
    ReadWrite<uint32_t> int2_srccfg;
    ReadWrite<uint32_t> int3_srccfg;
    Reserved<uint32_t> _reserved2[1];
    ReadOnly<uint32_t> int0_srcflg;
    WriteOnly<uint32_t> int0_clrflg;
    Reserved<uint32_t> _reserved3[954];
    ReadOnly<uint32_t> stat;
    WriteOnly<uint32_t> cfg;
    ReadWrite<uint32_t> ctlbase;
    ReadOnly<uint32_t> altbase;
    ReadOnly<uint32_t> waitstat;
    WriteOnly<uint32_t> swreq;
    ReadWrite<uint32_t> useburstset;
    WriteOnly<uint32_t> useburstclr;
    ReadWrite<uint32_t> reqmaskset;
    WriteOnly<uint32_t> reqmaskclr;
    ReadWrite<uint32_t> enaset;
    WriteOnly<uint32_t> enaclr;
    ReadWrite<uint32_t> altset;
    WriteOnly<uint32_t> altclr;
    ReadWrite<uint32_t> prioset;
    WriteOnly<uint32_t> prioclr;
    Reserved<uint32_t> _reserved4[3];
    ReadWrite<uint32_t> errclr;
};
#pragma pack()

static_assert(std::is_standard_layout<DmaRegisters>::value,
    "DmaRegisters isn't standard layout");

constexpr size_t DMA_CHANNEL_CNT = 8;

#pragma pack(1)
class DmaChannelControl {
public:
    constexpr explicit DmaChannelControl() noexcept
    : src_ptr(U32_0), dst_ptr(U32_0), ctrl(U32_0), _unused(U32_0) {}

    DmaChannelControl(DmaChannelControl&) = delete;
    DmaChannelControl(DmaChannelControl&&) = delete;

    InMemory<uint32_t> src_ptr;
    InMemory<uint32_t> dst_ptr;
    InMemory<uint32_t> ctrl;
    InMemory<uint32_t> _unused;
private:
    static constexpr uint32_t U32_0 = 0;
};
#pragma pack()

class DmaControl {
public:
    constexpr explicit DmaControl() noexcept
        : dma_ctrl{{
            DmaChannelControl{}, DmaChannelControl{},
            DmaChannelControl{}, DmaChannelControl{},
            DmaChannelControl{}, DmaChannelControl{},
            DmaChannelControl{}, DmaChannelControl{},
            DmaChannelControl{}, DmaChannelControl{},
            DmaChannelControl{}, DmaChannelControl{},
            DmaChannelControl{}, DmaChannelControl{},
            DmaChannelControl{}, DmaChannelControl{}}} {}
    constexpr DmaChannelControl& operator[](size_t idx) noexcept { return dma_ctrl[idx]; }

private:
    std::array<DmaChannelControl, 2 * DMA_CHANNEL_CNT> dma_ctrl;
} __attribute__ ((aligned (256)));

static_assert(std::is_standard_layout<DmaChannelControl>::value,
    "DmaChannelControl isn't standard layout");
static_assert(sizeof(DmaChannelControl) == 0x10, "DmaChannelControl has an invalid size");

constexpr size_t DMA_BASE = 0x4000E000;

namespace dmactrl {
    namespace ctrl {
        constexpr BitField<uint32_t> cycle_ctrl{2, 0};
        constexpr BitField<uint32_t> next_useburst{3, 3};
        constexpr BitField<uint32_t> n_minus_1{13, 4};
        constexpr BitField<uint32_t> r_power{17, 14};
        constexpr BitField<uint32_t> src_prot_ctr{20, 18};
        constexpr BitField<uint32_t> dst_prot_ctr{23, 21};
        constexpr BitField<uint32_t> src_size{25, 24};
        constexpr BitField<uint32_t> src_inc{27, 26};
        constexpr BitField<uint32_t> dst_size{29, 28};
        constexpr BitField<uint32_t> dst_inc{31, 30};
    }
}

namespace dmaregs {
    namespace device_cfg {
        constexpr BitField<uint32_t> num_dma_channels{7, 0};
        constexpr BitField<uint32_t> num_src_per_channel{15, 8};
    }
    namespace intx_srccfg {
        constexpr BitField<uint32_t> int_src{4, 0};
        constexpr BitField<uint32_t> en{5, 5};
    }
    namespace chan {
        constexpr BitField<uint32_t> ch0{0, 0};
        constexpr BitField<uint32_t> ch1{1, 1};
        constexpr BitField<uint32_t> ch2{2, 2};
        constexpr BitField<uint32_t> ch3{3, 3};
        constexpr BitField<uint32_t> ch4{4, 4};
        constexpr BitField<uint32_t> ch5{5, 5};
        constexpr BitField<uint32_t> ch6{6, 6};
        constexpr BitField<uint32_t> ch7{7, 7};
        constexpr BitField<uint32_t> ch8{8, 8};
        constexpr BitField<uint32_t> ch9{9, 9};
        constexpr BitField<uint32_t> ch10{10, 10};
        constexpr BitField<uint32_t> ch11{11, 11};
        constexpr BitField<uint32_t> ch12{12, 12};
        constexpr BitField<uint32_t> ch13{13, 13};
        constexpr BitField<uint32_t> ch14{14, 14};
        constexpr BitField<uint32_t> ch15{15, 15};
        constexpr BitField<uint32_t> ch16{16, 16};
        constexpr BitField<uint32_t> ch17{17, 17};
        constexpr BitField<uint32_t> ch18{18, 18};
        constexpr BitField<uint32_t> ch19{19, 19};
        constexpr BitField<uint32_t> ch20{20, 20};
        constexpr BitField<uint32_t> ch21{21, 21};
        constexpr BitField<uint32_t> ch22{22, 22};
        constexpr BitField<uint32_t> ch23{23, 23};
        constexpr BitField<uint32_t> ch24{24, 24};
        constexpr BitField<uint32_t> ch25{25, 25};
        constexpr BitField<uint32_t> ch26{26, 26};
        constexpr BitField<uint32_t> ch27{27, 27};
        constexpr BitField<uint32_t> ch28{28, 28};
        constexpr BitField<uint32_t> ch29{29, 29};
        constexpr BitField<uint32_t> ch30{30, 30};
        constexpr BitField<uint32_t> ch31{31, 31};
    }
    namespace stat {
        constexpr BitField<uint32_t> masten{0, 0};
        constexpr BitField<uint32_t> state{7, 4};
        constexpr BitField<uint32_t> dmachans{20, 16};
        constexpr BitField<uint32_t> teststat{31, 28};
    }
    namespace cfg {
        constexpr BitField<uint32_t> masten{0, 0};
        constexpr BitField<uint32_t> chprotctrl{7, 5};
    }
    namespace ctlbase {
        constexpr BitField<uint32_t> addr{31, 5};
    }
    namespace altbase {
        constexpr BitField<uint32_t> addr{31, 0};
    }
    namespace errclr {
        constexpr BitField<uint32_t> errclr{0, 0};
    }
    namespace ch_srccfg {
        constexpr BitField<uint32_t> dma_src{7, 0};
    }
}
