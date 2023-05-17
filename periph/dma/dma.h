// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <array>

#include "err.h"
#include "helpers.h"
#include "register.h"

#include "dma_regs.h"

// DMA Channel/Source Mapping according to the MSP432P401R userguide (https://www.ti.com/lit/ds/slas826e/slas826e.pdf)
// Note: The reserved channels can be used to transfer data from memory to memory.
//
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// |           | SRC = 0  | SRC = 1   | SRC = 2    | SRC = 3    | SRC = 4    | SRC = 5    | SRC = 6 | SRC = 7              |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// | Channel 0 | Reserved | USCIA0_TX | USCIB0_TX0 | USCIB3_TX1 | USCIB2_TX2 | USCIB1_TX3 | TA0CCR0 | AES256_Trigger0      |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// | Channel 1 | Reserved | USCIA0_RX | USCIB0_RX0 | USCIB3_RX1 | USCIB2_RX2 | USCIB1_RX3 | TA0CCR2 | AES256_Trigger1      |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// | Channel 2 | Reserved | USCIA1_TX | USCIB1_TX0 | USCIB0_TX1 | USCIB3_TX2 | USCIB2_TX3 | TA1CCR0 | AES256_Trigger2      |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// | Channel 3 | Reserved | USCIA1_RX | USCIB1_RX0 | USCIB0_RX1 | USCIB3_RX2 | USCIB2_RX3 | TA1CCR2 | Reserved             |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// | Channel 4 | Reserved | USCIA2_TX | USCIB2_TX0 | USCIB1_TX1 | USCIB0_TX2 | USCIB3_TX3 | TA2CCR0 | Reserved             |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// | Channel 5 | Reserved | USCIA2_RX | USCIB2_RX0 | USCIB1_RX1 | USCIB0_RX2 | USCIB3_RX3 | TA2CCR2 | Reserved             |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// | Channel 6 | Reserved | USCIA3_TX | USCIB3_TX0 | USCIB2_TX1 | USCIB1_TX2 | USCIB0_TX3 | TA3CCR0 | DMAE0 (External Pin) |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+
// | Channel 7 | Reserved | USCIA3_RX | USCIB3_RX0 | USCIB2_RX1 | USCIB1_RX2 | USCIB0_RX3 | TA3CCR2 | ADC                  |
// +-----------+----------+-----------+------------+------------+------------+------------+---------+----------------------+

// Although there are 8bits reserved for selecting a source for the DMA trigger, the
// MSP432P4x family only supports numbers from 1 to 7. 0 doesn't cause an error, but it's
// marked as reserved.
constexpr uint8_t MAX_SRC_NR = 7;

enum class DmaMode : uint8_t {
    Basic = 1,
    AutoRequest = 2,
    PingPong = 3,
    MemoryScatterGather = 4,
    PeripheralScatterGather = 6,
};

enum class DmaTransferType : uint8_t {
    PeripheralToMemory,
    PeripheralToMemoryPingPong,
    MemoryToPeripheral,
    MemoryToMemory,
    Custom,
    None,
};

enum class DmaDataWidth : uint8_t {
    Width8Bit = 0,
    Width16Bit,
    Width32Bit,
};

enum class DmaPtrIncrement : uint8_t {
    Incr8Bit = 0,
    Incr16Bit,
    Incr32Bit,
    NoIncr,
};

struct DmaConfig {
    constexpr explicit DmaConfig(uint8_t src_chan, DmaDataWidth width, DmaPtrIncrement src_incr,
        DmaPtrIncrement dst_incr, void* instance,
        void (*cb)(const uint8_t* src_buf, uint8_t* dst_buf, size_t len, void* instance)) noexcept
        : src_chan(src_chan), width(width), src_incr(src_incr), dst_incr(dst_incr),
        instance(instance), done(cb) {}

    constexpr explicit DmaConfig() noexcept
        : src_chan(0), width(DmaDataWidth::Width8Bit), src_incr(DmaPtrIncrement::Incr8Bit),
        dst_incr(DmaPtrIncrement::Incr8Bit), instance(nullptr), done(nullptr) {}

    uint8_t src_chan;
    DmaDataWidth width;
    DmaPtrIncrement src_incr;
    DmaPtrIncrement dst_incr;

    void* instance;
    void (*done)(const uint8_t* src_buf, uint8_t* dst_buf, size_t len, void* instance);
};

class DmaChannel {
public:
    Err setup(const DmaConfig& conf) noexcept;
    bool transfer_going() noexcept { return info.busy; }
    Err transfer_mem_to_periph(const uint8_t* src, uint8_t* dst, uint32_t len) noexcept;
    Err transfer_periph_to_mem(const uint8_t* src, uint8_t* dst, uint32_t len) noexcept;
    Err transfer_custom(const uint8_t* src, uint8_t* dst, DmaPtrIncrement src_incr,
        DmaPtrIncrement dst_incr, uint32_t len) noexcept;

    friend class Dma;
private:
    static constexpr size_t MAX_TRANSFERS_LEN = 1024;
    struct TransferInfo {
        const uint8_t* src;
        uint8_t* dst;
        size_t num_bytes;
        size_t remaining_words;
        DmaTransferType type;
        bool busy;

        constexpr explicit TransferInfo() noexcept
            : src(nullptr), dst(nullptr), num_bytes(0), remaining_words(0),
            type(DmaTransferType::None), busy(false) {}
    };

    constexpr explicit DmaChannel(uint8_t idx, DmaChannelControl& prim, DmaChannelControl& alt)
        noexcept : idx(idx), reg_addr(DMA_BASE), ctrl_prim(prim), ctrl_alt(alt),
        mode(DmaMode::Basic), info(), conf(), in_use(false) {}

    inline DmaRegisters& reg() const noexcept
    {
        return *reinterpret_cast<DmaRegisters*>(reg_addr);
    }

    void handle_interrupt() noexcept;
    void calc_remaining_words(size_t num_bytes) noexcept;
    void update_dma_pointers() noexcept;
    void enable_channel() noexcept;
    void config_prim_channel(uint32_t bytes_to_transmit) noexcept;

    const uint8_t idx;
    const size_t reg_addr;
    DmaChannelControl& ctrl_prim;
    DmaChannelControl& ctrl_alt;

    DmaMode mode;
    DmaChannel::TransferInfo info;
    DmaConfig conf;
    bool in_use;
};

class Dma {
public:
    Dma(const Dma&) = delete;
    Dma(const Dma&&) = delete;
    Dma& operator=(const Dma&) = delete;
    Dma& operator=(const Dma&&) = delete;
    constexpr ~Dma() noexcept {}
    void init() noexcept;

    // FIXME: implement a way to check if the channel is already in use
    constexpr DmaChannel& operator[](uint8_t chan) noexcept { return dma_channels[chan]; }

    friend class Msp432;
    friend void periph_int_handler(void) noexcept;
private:
    constexpr explicit Dma() noexcept
        : reg_addr(DMA_BASE), dma_ctrl(), dma_channels{
        DmaChannel{0, dma_ctrl[0], dma_ctrl[0 + DMA_CHANNEL_CNT]},
        DmaChannel{1, dma_ctrl[1], dma_ctrl[1 + DMA_CHANNEL_CNT]},
        DmaChannel{2, dma_ctrl[2], dma_ctrl[2 + DMA_CHANNEL_CNT]},
        DmaChannel{3, dma_ctrl[3], dma_ctrl[3 + DMA_CHANNEL_CNT]},
        DmaChannel{4, dma_ctrl[4], dma_ctrl[4 + DMA_CHANNEL_CNT]},
        DmaChannel{5, dma_ctrl[5], dma_ctrl[5 + DMA_CHANNEL_CNT]},
        DmaChannel{6, dma_ctrl[6], dma_ctrl[6 + DMA_CHANNEL_CNT]},
        DmaChannel{7, dma_ctrl[7], dma_ctrl[7 + DMA_CHANNEL_CNT]}} {}

    void handle_interrupt(int num) noexcept;

    inline DmaRegisters& reg() const noexcept
    {
        return *reinterpret_cast<DmaRegisters*>(reg_addr);
    }

    const size_t reg_addr;
    DmaControl dma_ctrl;
    std::array<DmaChannel, DMA_CHANNEL_CNT> dma_channels;
};
