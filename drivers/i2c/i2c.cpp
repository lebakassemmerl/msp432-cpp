// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#include <atomic>
#include <cstdint>
#include <span>

#include "cs.h"
#include "err.h"
#include "i2c.h"
#include "usci.h"
#include "uscib_regs.h"

Err I2cMaster::init(const Cs& clk) noexcept
{
    if (initialized)
        return Err::AlreadyInitialized;

    usci.reg().ctlw0.set(1); // disable module and reset all other settings

    // set prescaler for clk-speed
    usci.reg().brw.set(static_cast<uint16_t>(clk.sm_clk() / static_cast<uint32_t>(speed)));

    usci.reg().ctlw1.modify(
        uscibregs::ctlw1::astp.value(0) + // enable automatic stop condition generation
        uscibregs::ctlw1::clto.value(0)   // disable clock low timeout counter
    );

    usci.reg().ctlw0.modify(
        uscibregs::ctlw0::mm.value(0) +     // single master environment
        uscibregs::ctlw0::mst.value(1) +    // Master mode
        uscibregs::ctlw0::mode.value(3) +   // I2C mode
        uscibregs::ctlw0::sync.value(0) +   // sync mode
        uscibregs::ctlw0::ssel.value(2) +   // use SMCLK as I2C clock-source
        uscibregs::ctlw0::swrst.value(0)    // disable software-reset -> enable module
    );

    return  Err::Ok;
}

Err I2cMaster::write(uint16_t addr,
        std::span<const uint8_t> data,
        void* cookie,
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie),
        bool addr_10bit) noexcept
{
    if (addr_10bit && (addr > 1023))
        return Err::OutOfRange;
    else if (addr > 127)
        return Err::OutOfRange;

    if (data.size() > 0xFFFF)
        return Err::Overflow;

    if (!initialized)
        return Err::NotInitialized;

    if (jobfifo.is_full())
        return Err::NoMem;

    jobfifo.emplace(
        I2cJobType::Write, addr, addr_10bit, data, std::span<uint8_t>{}, cookie, finished);

    if (transmitting.load(std::memory_order_relaxed))
        return Err::Ok;

    transmitting.store(true, std::memory_order_release);

    start_job(addr, addr_10bit, false, static_cast<uint16_t>(data.size()));

    return Err::Ok;
}

Err I2cMaster::read(uint16_t addr,
        std::span<uint8_t> buffer,
        void* cookie,
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie),
        bool addr_10bit) noexcept
{
    if (addr_10bit && (addr > 1023))
        return Err::OutOfRange;
    else if (addr > 127)
        return Err::OutOfRange;

    if (buffer.size() > 0xFFFF)
        return Err::Overflow;

    if (!initialized)
        return Err::NotInitialized;

    if (jobfifo.is_full())
        return Err::NoMem;

    jobfifo.emplace(
        I2cJobType::Read, addr, addr_10bit, std::span<uint8_t>{}, buffer, cookie, finished);

    if (transmitting.load(std::memory_order_relaxed))
        return Err::Ok;

    transmitting.store(true, std::memory_order_release);

    start_job(addr, addr_10bit, true, static_cast<uint16_t>(buffer.size()));

    return Err::Ok;
}

Err I2cMaster::write_read(uint16_t addr,
        std::span<const uint8_t> txbuf,
        std::span<uint8_t> rxbuf,
        void* cookie,
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie),
        bool addr_10bit) noexcept
{
    uint32_t txrx_bytes;

    if (addr_10bit && (addr > 1023))
        return Err::OutOfRange;
    else if (addr > 127)
        return Err::OutOfRange;

    txrx_bytes = txbuf.size() + rxbuf.size();
    if (txrx_bytes > 0xFFFF)
        return Err::Overflow;

    if (!initialized)
        return Err::NotInitialized;

    if (jobfifo.is_full())
        return Err::NoMem;

    jobfifo.emplace(I2cJobType::WriteRead, addr, addr_10bit, txbuf, rxbuf, cookie, finished);

    if (transmitting.load(std::memory_order_relaxed))
        return Err::Ok;

    transmitting.store(true, std::memory_order_release);

    start_job(addr, addr_10bit, false, static_cast<uint16_t>(txrx_bytes));

    return Err::Ok;
}


void I2cMaster::handle_interrupt() noexcept
{
    uint16_t iflags = usci.reg().ifg.get();
    usci.reg().ifg.set(0);

    I2cJob& job = jobfifo.peek_ref().value().get();

    auto handle_err = [&] (I2cErr err) {
        if (job.retry_cnt >= I2cJob::MAX_RETRIES) {
            // invoke callback
            if (job.finished)
                job.finished(job.type, err, job.rxbuf, job.cookie);

            // drop failed job from FIFO
            this->jobfifo.pop();
            if (this->jobfifo.is_empty())
                return;

            I2cJob& new_job = this->jobfifo.peek_ref().value().get();
            uint16_t txrx_bytes =
                static_cast<uint16_t>(new_job.txbuf.size() + new_job.rxbuf.size());
            this->start_job(
                new_job.addr, new_job.addr_10bit, new_job.type == I2cJobType::Read, txrx_bytes);
        } else {
            uint16_t txrx_bytes = static_cast<uint16_t>(job.txbuf.size() + job.rxbuf.size());

            job.buf_idx = 0;
            job.retry_cnt += 1;
            this->start_job(job.addr, job.addr_10bit, job.type == I2cJobType::Read, txrx_bytes);
        }
    };

    if (iflags & uscibregs::ifg::nackifg.mask()) {
        handle_err(I2cErr::Nack);
    } else if (iflags & uscibregs::ifg::alifg.mask()) {
        handle_err(I2cErr::ArbitrationLost);
    } else if (iflags & uscibregs::ifg::cltoifg.mask()) {
        handle_err(I2cErr::ClockLowTimeout);
    } else if (iflags & uscibregs::ifg::rxifg0.mask()) {
        // we know, that we are in receive-mode since the RX interrupt flag was set

        if (job.buf_idx == (job.rxbuf.size() - 1)) {
            // we are at the last byte to receive -> wait for stop-condition interrupt
            usci.reg().ie.set(
                uscibregs::ifg::nackifg.value(1) +  // enable NACK interrupt
                uscibregs::ifg::alifg.value(1) +    // enable arbitration lost interrupt
                uscibregs::ifg::cltoifg.value(1) +  // enable 'clock-low-timeout' interrupt
                uscibregs::ifg::stpifg.value(1)     // enable stop-condition interrupt
            );
        }

        job.rxbuf[job.buf_idx] = static_cast<uint8_t>(usci.reg().rxbuf.get());
        job.buf_idx += 1;
    } else if (iflags & uscibregs::ifg::txifg0.mask()) {
        // we know, that we are in transmit-mode since the TX interrupt flag was set
        if (job.buf_idx < (job.txbuf.size() - 1)) {
            // we have to keep transmitting
            usci.reg().txbuf.set(job.txbuf[job.buf_idx]);
            job.buf_idx += 1;
        } else if (job.buf_idx < (job.txbuf.size() - 1)) {
            // we are at the last byte to transmit
            if (job.type == I2cJobType::Write) {
                // there is no read-part left, simply wait for the stop-condition
                usci.reg().ie.set(
                    uscibregs::ifg::nackifg.value(1) +  // enable NACK interrupt
                    uscibregs::ifg::alifg.value(1) +    // enable arbitration lost interrupt
                    uscibregs::ifg::cltoifg.value(1) +  // enable 'clock-low-timeout' interrupt
                    uscibregs::ifg::stpifg.value(1)     // enable stop-condition interrupt
                );
            }

            // write the actual data to the tx-register
            usci.reg().txbuf.set(job.txbuf[job.buf_idx]);
            job.buf_idx += 1;
        } else {
            // If we land here, we have a WriteRead job where the write-part has finished
            // -> switch to the read-part.

            // reset the buffer-index
            job.buf_idx = 0;

            // enable the desired interrupts
            usci.reg().ie.set(
                uscibregs::ifg::nackifg.value(1) +  // enable NACK interrupt
                uscibregs::ifg::alifg.value(1) +    // enable arbitration lost interrupt
                uscibregs::ifg::cltoifg.value(1) +  // enable 'clock-low-timeout' interrupt
                uscibregs::ifg::rxifg0.value(1)     // enable rx-register empty interrupt
            );

            usci.reg().ctlw0.modify(
                uscibregs::ctlw0::tr.value(0) +     // switch from transmitter to receiver
                uscibregs::ctlw0::txstt.value(1)    // issue the repeated-start-condition
            );
        }
    } else if (iflags & uscibregs::ifg::stpifg.mask()) {
        // stop-condition interrupt, which means our job (no matter what type) has finished

        // disable all interrupt flags, in case we don't have any pending jobs
        usci.reg().ifg.set(0);

        // invoke the job-callback (if registered)
        if (job.finished)
            job.finished(job.type, I2cErr::Ok, job.rxbuf, job.cookie);

        // drop job from FIFO
        jobfifo.pop();

        // check if any jobs are pending
        if (jobfifo.is_empty()) {
            // no pending jobs
            transmitting.store(false, std::memory_order_release);
        } else {
            I2cJob& job = jobfifo.peek_ref().value().get();
            start_job(job.addr, job.addr_10bit, job.type ==I2cJobType::Read,
                static_cast<uint16_t>(job.txbuf.size() + job.rxbuf.size()));
        }
    }
}
