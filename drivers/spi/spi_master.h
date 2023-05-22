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

template<typename T> requires UsciReg<T>
class SpiMaster {
public:
    consteval explicit SpiMaster(Usci<T>& usci, Dma& dma, SpiMode mode, uint32_t freq_hz,
        uint8_t tx_dma_chan, uint8_t rx_dma_chan, uint8_t tx_dma_src, uint8_t rx_dma_src) noexcept
        : initialized(false), transm_going(false), desired_freq(freq_hz), actual_freq(0),
        mode(mode), usci(usci), tx_dma(dma[tx_dma_chan]), rx_dma(dma[rx_dma_chan]),
        tx_dma_src(tx_dma_src), rx_dma_src(rx_dma_src), job_fifo() {}

    Err init(const Cs& cs) noexcept
    {
        Err ret;
        uint16_t pol = static_cast<uint16_t>(mode) & 0x01;
        uint16_t ph = (static_cast<uint16_t>(mode) & 0x02) >> 1;
        uint32_t smclk = cs.sm_clk();

        // calculate the nearest value of the desired frequency
        int32_t div = static_cast<int32_t>(smclk / desired_freq);
        int32_t val1 = (smclk / (div + 1)) - desired_freq;
        int32_t val2 = (smclk / div) - desired_freq;
        if (val1 < 0)
            val1 = -val1;

        if (val1 < val2)
            div += 1;

        uint16_t brw = static_cast<uint16_t>(div);
        actual_freq = smclk / static_cast<uint32_t>(brw);

        usci.ctlw0().set(uscispiregs::ctlw0::swrst.value(1)); // disable the module

        // setup the module configuration
        usci.ctlw0().modify(
            uscispiregs::ctlw0::stem.value(0)       // don't use STE pin
            + uscispiregs::ctlw0::ssel.value(3)     // use SMCLK as clock source
            + uscispiregs::ctlw0::sync.value(1)     // use synchronous mode
            + uscispiregs::ctlw0::mode.value(0)     // use 3-Wire SPI mode
            + uscispiregs::ctlw0::mst.value(1)      // master mode
            + uscispiregs::ctlw0::sevenbit.value(0) // use 8bit mode
            + uscispiregs::ctlw0::msb.value(1)      // transmit MSB first
            + uscispiregs::ctlw0::ckpl.value(pol)   // set clock polarity
            + uscispiregs::ctlw0::ckph.value(ph)    // set clock phase
        );

        usci.brw().set(brw);
        usci.ctlw0().modify(uscispiregs::ctlw0::swrst.value(0));

        ret = tx_dma.setup(DmaConfig{
            tx_dma_src,
            DmaDataWidth::Width8Bit,
            DmaPtrIncrement::Incr8Bit,
            DmaPtrIncrement::NoIncr,
            reinterpret_cast<void*>(this),
            SpiMaster::redirect_int_handler
        });

        if (ret != Err::Ok)
            return ret;

        ret = rx_dma.setup(DmaConfig{
            rx_dma_src,
            DmaDataWidth::Width8Bit,
            DmaPtrIncrement::NoIncr,
            DmaPtrIncrement::Incr8Bit,
            reinterpret_cast<void*>(this),
            SpiMaster::redirect_int_handler
        });
        if (ret != Err::Ok)
            return ret;

        initialized = true;
        return Err::Ok;
    }

    Err write(std::span<uint8_t> data, void* context, SpiCallback cb) noexcept
    {
        if (data.empty() == 0)
            return Err::Empty;

        if (job_fifo.free() == 0)
            return Err::NoMem;

        job_fifo.emplace(SpiJob{
            SpiTransferType::Write,
            data.data(),
            nullptr,
            data.size(),
            context,
            cb
        });

        if (!transm_going) {
            transm_going = true;
            start_transmission();
        }

        return Err::Ok;
    }

    Err read(std::span<uint8_t> buffer, void* context, SpiCallback cb) noexcept
    {
        if (buffer.empty() == 0)
            return Err::Empty;

        if (job_fifo.free() == 0)
            return Err::NoMem;

        job_fifo.emplace(SpiJob{
            SpiTransferType::Read,
            nullptr,
            buffer.data,
            buffer.size(),
            context,
            cb
        });

        if (!transm_going) {
            transm_going = true;
            start_transmission();
        }

        return Err::Ok;
    }

    Err write_read(std::span<uint8_t> txbuf, std::span<uint8_t> rxbuf, void* context,
        SpiCallback cb) noexcept
    {
        if (txbuf.empty() || rxbuf.empty())
            return Err::Empty;

        if (job_fifo.free() == 0)
            return Err::NoMem;

        job_fifo.emplace(SpiJob{
            SpiTransferType::WriteRead,
            txbuf.data(),
            rxbuf.data(),
            std::min(txbuf.size(), rxbuf.size()),
            context,
            cb
        });

        if (!transm_going) {
            transm_going = true;
            start_transmission();
        }

        return Err::Ok;
    }

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
        const uint8_t* src_buf, uint8_t* dst_buf, size_t len, void* instance) noexcept
    {
        SpiMaster* inst = reinterpret_cast<SpiMaster*>(instance);
        inst->int_handler(src_buf, dst_buf, len);
    }

    void start_transmission() noexcept
    {
        const uint8_t* rxreg = reinterpret_cast<uint8_t*>(&usci.rxbuf());
        uint8_t* txreg = reinterpret_cast<uint8_t*>(&usci.txbuf());
        SpiJob& job = job_fifo.peek_ref().value().get();

        switch (job.type) {
        case SpiTransferType::Write:
            tx_dma.transfer_mem_to_periph(job.txbuf, txreg, job.len);
            break;
        case SpiTransferType::Read:
            rx_dma.transfer_periph_to_mem(rxreg, job.rxbuf, job.len);
            // We want to transmit the same idle byte 'job.len' times since SPI is must also write data
            // to the bus although it's only reading.
            tx_dma.transfer_custom(&TX_IDLE, txreg, DmaPtrIncrement::NoIncr,
                DmaPtrIncrement::NoIncr, job.len);
            break;
        case SpiTransferType::WriteRead:
            rx_dma.transfer_periph_to_mem(rxreg, job.rxbuf, job.len);
            tx_dma.transfer_mem_to_periph(job.txbuf, txreg, job.len);
            break;
        default:
            // should never happen!
            break;
        }
    }
    void int_handler(const uint8_t* src_buf, uint8_t* dst_buf, size_t len) noexcept
    {
        SpiJob& job = job_fifo.peek_ref().value().get();

        if (((job.type == SpiTransferType::Read) && (src_buf == &TX_IDLE)) ||
            ((job.type == SpiTransferType::WriteRead) && (src_buf == job.txbuf))) {
            // We got the TX interrupt of either a read- or a read-write-transmission. We do not
            // need this interrupt since we have to wait for the RX interrupt anyway -> simply
            // return from the handler.
            return;
        }

        if (job.cb != nullptr) {
            switch (job.type) {
            case SpiTransferType::Write:
                job.cb(job.type, std::span{job.txbuf, len}, std::span<uint8_t>{}, job.context);
                break;
            case SpiTransferType::Read:
                job.cb(job.type, std::span<uint8_t>{}, std::span{job.rxbuf, len}, job.context);
                break;
            case SpiTransferType::WriteRead:
                job.cb(job.type, std::span{job.txbuf, len}, std::span{job.rxbuf, len}, job.context);
                break;
            default:
                break;
            }
        }

        job_fifo.pop();
        if (job_fifo.is_empty())
            transm_going = false;
        else
            start_transmission();
    }

    bool initialized;
    bool transm_going;
    uint32_t desired_freq;
    uint32_t actual_freq;
    SpiMode mode;
    Usci<T>& usci;

    DmaChannel& tx_dma;
    DmaChannel& rx_dma;
    uint8_t tx_dma_src;
    uint8_t rx_dma_src;

    Fifo<SpiJob, 16> job_fifo;
};
