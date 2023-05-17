// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>
#include <span>

#include "cs.h"
#include "err.h"
#include "dma_fifo.h"
#include "pin.h"
#include "usci.h"
#include "uscia_regs.h"

class Uart {
public:
    consteval explicit Uart(Usci<UsciARegisters>& usci, Dma& dma, size_t baud, uint8_t tx_dma_chan,
        uint8_t rx_dma_chan, uint8_t tx_dma_src, uint8_t rx_dma_src) noexcept
        : initialized(false), tx_fifo(), usci(usci), baud(baud), tx_dma(dma[tx_dma_chan]),
        rx_dma(dma[rx_dma_chan]), tx_dma_src(tx_dma_src), rx_dma_src(rx_dma_src) {}

    Err init(const Cs& cs) noexcept;
    Err write(std::span<uint8_t> data) noexcept;
private:
    static void redirect_tx_handler(
        const uint8_t* src_buf, uint8_t* dst_buf, size_t len, void* instance) noexcept;
    static void redirect_rx_handler(
        const uint8_t* src_buf, uint8_t* dst_buf, size_t len, void* instance) noexcept;
    void tx_handler(const uint8_t* buf, size_t len) noexcept;
    void rx_handler(uint8_t* buf, size_t len) noexcept;

    void queue_tx_job() noexcept;

    bool initialized;
    DmaFifo<uint8_t, 512> tx_fifo;
    Usci<UsciARegisters>& usci;
    uint32_t baud;

    DmaChannel& tx_dma;
    DmaChannel& rx_dma;
    uint8_t tx_dma_src;
    uint8_t rx_dma_src;
};
