// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>
#include <span>

#include "cs.h"
#include "dma.h"
#include "err.h"
#include "fifo.h"
#include "spi.h"
#include "usci.h"
#include "uscispi.h"

static const uint8_t TX_IDLE = 0xFF;

class SpiMaster {
public:
    consteval explicit SpiMaster(Usci& usci, Dma& dma, SpiMode mode, uint32_t freq_hz,
        uint8_t tx_dma_chan, uint8_t rx_dma_chan, uint8_t tx_dma_src, uint8_t rx_dma_src) noexcept
        : initialized(false), transm_going(false), desired_freq(freq_hz), actual_freq(0),
        mode(mode), usci(usci), tx_dma(dma[tx_dma_chan]), rx_dma(dma[rx_dma_chan]),
        tx_dma_src(tx_dma_src), rx_dma_src(rx_dma_src), job_fifo() {}

    Err init(const Cs& cs) noexcept;

    Err write(std::span<uint8_t> data, void* context, SpiCallback cb) noexcept;
    Err read(std::span<uint8_t> buffer, void* context, SpiCallback cb) noexcept;
    Err write_read(std::span<uint8_t> txbuf, std::span<uint8_t> rxbuf, void* context,
        SpiCallback cb) noexcept;

    uint32_t get_actual_freq_hz() const noexcept { return actual_freq; }
    uint32_t get_desired_freq_hz() const noexcept { return desired_freq; }
private:
    struct SpiJob {
        SpiTransferType type;
        uint8_t* txbuf;
        uint8_t* rxbuf;
        size_t len;
        void* context;
        SpiCallback cb;

        constexpr explicit SpiJob() noexcept
            : type(SpiTransferType::None), txbuf(nullptr), rxbuf(nullptr), len(0), context(nullptr),
            cb(nullptr) {}

        constexpr explicit SpiJob(SpiTransferType type, uint8_t* txbuf, uint8_t* rxbuf, size_t len,
            void* context, SpiCallback cb) noexcept
            : type(SpiTransferType::None), txbuf(nullptr), rxbuf(nullptr), len(0), context(nullptr),
            cb(nullptr) {}
    };

    static void redirect_int_handler(
        const uint8_t* src_buf, uint8_t* dst_buf, size_t len, void* instance) noexcept;
    void start_transmission() noexcept;
    void int_handler(const uint8_t* src_buf, uint8_t* dst_buf, size_t len) noexcept;

    bool initialized;
    bool transm_going;
    uint32_t desired_freq;
    uint32_t actual_freq;
    SpiMode mode;
    Usci& usci;

    DmaChannel& tx_dma;
    DmaChannel& rx_dma;
    uint8_t tx_dma_src;
    uint8_t rx_dma_src;

    Fifo<SpiJob, 16> job_fifo;
};
