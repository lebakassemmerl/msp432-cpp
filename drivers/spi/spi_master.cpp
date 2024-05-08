// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <cstdint>
#include <span>

#include "cs.h"
#include "dma.h"
#include "err.h"
#include "fifo.h"
#include "spi.h"
#include "spi_master.h"
#include "usci.h"
#include "uscispi.h"

#include "register.h"

static const uint8_t TX_IDLE = 0xFF;

Err SpiMaster::init(const Cs& cs) noexcept
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
        [] (const uint8_t* src, uint8_t* dst, size_t len, void* inst) noexcept -> void {}
    });

    if (ret != Err::Ok)
        return ret;

    ret = rx_dma.setup(DmaConfig{
        rx_dma_src,
        DmaDataWidth::Width8Bit,
        DmaPtrIncrement::NoIncr,
        DmaPtrIncrement::Incr8Bit,
        reinterpret_cast<void*>(this),
        [] (const uint8_t* src, uint8_t *dst, size_t len, void* inst) noexcept -> void {
            SpiMaster* m = reinterpret_cast<SpiMaster*>(inst);
            m->int_handler(src, dst, len);
        },
    });

    if (ret != Err::Ok)
        return ret;

    initialized = true;
    return Err::Ok;
}

Err SpiMaster::write(std::span<const uint8_t> data, Pin* cs, void* context, SpiCallback cb) noexcept
{
    if (data.empty())
        return Err::Empty;

    if (job_fifo.free() == 0)
        return Err::NoMem;

    job_fifo.emplace(
        SpiTransferType::Write,
        const_cast<uint8_t*>(data.data()),
        nullptr,
        data.size(),
        cs,
        context,
        cb
    );

    if (!transm_going) {
        transm_going = true;
        start_transmission();
    }

    return Err::Ok;
}

Err SpiMaster::read(std::span<uint8_t> buffer, Pin* cs, void* context, SpiCallback cb) noexcept
{
    if (buffer.empty() == 0)
        return Err::Empty;

    if (job_fifo.free() == 0)
        return Err::NoMem;

    job_fifo.emplace(
        SpiTransferType::Read,
        nullptr,
        buffer.data(),
        buffer.size(),
        cs,
        context,
        cb
    );

    if (!transm_going) {
        transm_going = true;
        start_transmission();
    }

    return Err::Ok;
}

Err SpiMaster::write_read(
    std::span<const uint8_t> txbuf,
    std::span<uint8_t> rxbuf,
    Pin* cs,
    void* context,
    SpiCallback cb) noexcept
{
    if (txbuf.empty() || rxbuf.empty())
        return Err::Empty;

    if (job_fifo.free() == 0)
        return Err::NoMem;

    job_fifo.emplace(
        SpiTransferType::WriteRead,
        const_cast<uint8_t*>(txbuf.data()),
        rxbuf.data(),
        std::min(txbuf.size(), rxbuf.size()),
        cs,
        context,
        cb
    );

    if (!transm_going) {
        transm_going = true;
        start_transmission();
    }

    return Err::Ok;
}

void SpiMaster::start_transmission() noexcept
{
    const uint8_t* rxreg = reinterpret_cast<uint8_t*>(&usci.rxbuf());
    uint8_t* txreg = reinterpret_cast<uint8_t*>(&usci.txbuf());
    SpiJob& job = job_fifo.peek_ref().value().get();

    // if a CS-pin was provided, we pull it low before starting the job
    if (job.cs) {
        job.cs->set_high();
    }

    switch (job.type) {
    case SpiTransferType::Write:
        tx_dma.transfer_mem_to_periph(job.txbuf, txreg, job.len);
        // We need here an RX-transfer since the TX-interrupt occurs before all the data is sent
        // out. This means, we cannot reliably use it for disabling a CS-pin. Thus, trigger a dummy
        // read.
        rx_dma.transfer_custom(rxreg, &rx_dummy, DmaPtrIncrement::NoIncr,
            DmaPtrIncrement::NoIncr, job.len);
        break;
    case SpiTransferType::Read:
        rx_dma.transfer_periph_to_mem(rxreg, job.rxbuf, job.len);
        // We want to transmit the same idle byte 'job.len' times since SPI is must also write
        // data to the bus although it's only reading.
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

void SpiMaster::int_handler(const uint8_t* src_buf, uint8_t* dst_buf, size_t len) noexcept
{
    SpiJob& job = job_fifo.peek_ref().value().get();

    // if a CS-pin was provided, we pull it high since the job has finished
    if (job.cs) {
        job.cs->set_low();
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

