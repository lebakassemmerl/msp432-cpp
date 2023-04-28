// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include "cs.h"
#include "dma.h"
#include "err.h"
#include "spi.h"
#include "spi_master.h"
#include "usci.h"
#include "uscispi.h"

static const uint8_t TX_IDLE = 0xFF;

template<typename T>
err::Err SpiMaster<T>::init(const Cs& cs) noexcept
{
    err::Err ret;
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

    usci.ctlw0.set(uscispiregs::ctlw0::swrst.value(1)); // disable the module

    // setup the module configuration
    usci.ctlw0.modify(
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

    usci.brw.set(brw);
    usci.ctlw0.modify(uscispiregs::ctlw0::swrst.value(0));

    DmaConfig tx_cfg{};
    tx_cfg.src_chan = tx_dma_src;
    tx_cfg.width = DmaDataWidth::Width8Bit;
    tx_cfg.src_incr = DmaPtrIncrement::Incr8Bit;
    tx_cfg.dst_incr = DmaPtrIncrement::NoIncr;
    tx_cfg.instance = reinterpret_cast<void*>(this);
    tx_cfg.done = SpiMaster::redirect_int_handler;

    DmaConfig rx_cfg{};
    rx_cfg.src_chan = rx_dma_src;
    rx_cfg.width = DmaDataWidth::Width8Bit;
    rx_cfg.src_incr = DmaPtrIncrement::NoIncr;
    rx_cfg.dst_incr = DmaPtrIncrement::Incr8Bit;
    rx_cfg.instance = reinterpret_cast<void*>(this);
    rx_cfg.done = SpiMaster::redirect_int_handler;

    ret = tx_dma.setup(tx_cfg);
    if (ret != err::Err::Ok)
        return ret;

    ret = rx_dma.setup(rx_cfg);
    if (ret != err::Err::Ok)
        return ret;

    initialized = true;
    return err::Err::Ok;
}

template<typename T>
err::Err SpiMaster<T>::write(std::span<uint8_t> data, void* context, SpiCallback cb) noexcept
{
    SpiJob job;

    if (data.empty() == 0)
        return err::Err::Empty;

    if (job_fifo.free() == 0)
        return err::Err::NoMem;

    job.type = SpiTransferType::Write;
    job.txbuf = data.data();
    job.len = data.size();
    job.context = context;
    job.cb = cb;
    job_fifo.push(job);

    if (!transm_going) {
        transm_going = true;
        start_transmission();
    }

    return err::Err::Ok;
}

template<typename T>
err::Err SpiMaster<T>::read(std::span<uint8_t> buffer, void* context, SpiCallback cb) noexcept
{
    SpiJob job;

    if (buffer.empty() == 0)
        return err::Err::Empty;

    if (job_fifo.free() == 0)
        return err::Err::NoMem;

    job.type = SpiTransferType::Read;
    job.rxdata = buffer.data();
    job.len = buffer.size();
    job.context = context;
    job.cb = cb;
    job_fifo.push(job);

    if (!transm_going) {
        transm_going = true;
        start_transmission();
    }

    return err::Err::Ok;
}

template<typename T>
err::Err SpiMaster<T>::write_read(std::span<uint8_t> txbuf, std::span<uint8_t> rxbuf,
    void* context, SpiCallback cb) noexcept
{
    SpiJob job;

    if (txbuf.empty() || rxbuf.empty())
        return err::Err::Empty;

    if (job_fifo.free() == 0)
        return err::Err::NoMem;

    job.type = SpiTransferType::Read;
    job.txbuf = txbuf.data();
    job.rxbuf = rxbuf.data();
    job.len = std::min(txbuf.size(), rxbuf.size());
    job.context = context;
    job.cb = cb;
    job_fifo.push(job);

    if (!transm_going) {
        transm_going = true;
        start_transmission();
    }

    return err::Err::Ok;
}
template<typename T>
void SpiMaster<T>::redirect_int_handler(const uint8_t* src_buf, uint8_t* dst_buf, size_t len,
    void* instance)
{
    SpiMaster* inst = reinterpret_cast<SpiMaster*>(instance);
    inst->int_handler(src_buf, dst_buf, len);
}

template<typename T>
void SpiMaster<T>::start_transmission() noexcept
{
    SpiJob& job = job_fifo.peek().second.value().get();

    switch (job.type) {
    case SpiTransferType::Write:
        tx_dma.transfer_mem_to_periph(job.txbuf, reinterpret_cast<uint8_t*>(&usci.txbuf), job.len);
        break;
    case SpiTransferType::Read:
        rx_dma.transfer_periph_to_mem(reinterpret_cast<uint8_t*>(&usci.rxbuf), job.rxbuf, job.len);
        // We want to transmit the same idle byte 'job.len' times since SPI is must also write data
        // to the bus although it's only reading.
        tx_dma.transfer_custom(&TX_IDLE, reinterpret_cast<uint8_t*>(&usci.txbuf),
            DmaPtrIncrement::NoIncr, DmaPtrIncrement::NoIncr, job.len);
        break;
    case SpiTransferType::WriteRead:
        rx_dma.transfer_periph_to_mem(reinterpret_cast<uint8_t*>(&usci.rxbuf), job.rxbuf, job.len);
        tx_dma.transfer_mem_to_periph(job.txbuf, reinterpret_cast<uint8_t*>(&usci.txbuf), job.len);
        break;
    default:
        // should never happen!
        break;
    }
}

template<typename T>
void SpiMaster<T>::int_handler(const uint8_t* src_buf, uint8_t* dst_buf, size_t len) noexcept
{
    SpiJob& job = job_fifo.peek().second.value().get();

    if (((job.type == SpiTransferType::Read) && (src_buf == &TX_IDLE)) ||
        ((job.type == SpiTransferType::WriteRead) && (src_buf == job.txdata))) {
        // We got the TX interrupt of either a read- or a read-write-transmission. We do not need
        // this interrupt since we have to wait for the RX interrupt anyway -> simply return from
        // the handler.
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
        }
    }

    job_fifo.pop();
    if (job_fifo.is_empty())
        transm_going = false;
    else
        start_transmission();
}
