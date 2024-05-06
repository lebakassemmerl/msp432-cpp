// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <span>
#include <unistd.h>

#include "cs.h"
#include "err.h"
#include "fifo.h"
#include "usci.h"
#include "uscib_regs.h"

enum class I2cErr : uint8_t  {
    Ok,
    Nack,
    ArbitrationLost,
    ClockLowTimeout,
};

enum class I2cSpeed : uint32_t {
    KHz100 = 100'000,
    KHz400 = 400'000,
};

enum class I2cJobType {
    Write,
    Read,
    WriteRead,
};

class I2cMaster {
public:
    constexpr explicit I2cMaster(UsciB& usci, I2cSpeed speed)
        : usci(usci), speed(speed), initialized(false), transmitting(false), jobfifo() {}

    Err init(const Cs& clk) noexcept;

    Err write(uint16_t addr,
        std::span<const uint8_t> data,
        void* cookie,
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie),
        bool addr_10bit = false) noexcept;

    Err read(uint16_t addr,
        std::span<uint8_t> buffer,
        void* cookie,
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie),
        bool addr_10bit = false) noexcept;

    Err write_read(uint16_t addr,
        std::span<const uint8_t> txbuf,
        std::span<uint8_t> rxbuf,
        void* cookie,
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie),
        bool addr_10bit = false) noexcept;

private:
    struct I2cJob {
        static constexpr uint8_t MAX_RETRIES = 3;

        I2cJobType type;
        uint16_t addr;
        std::span<uint8_t> txbuf;
        std::span<uint8_t> rxbuf;
        bool addr_10bit;

        bool reading;
        uint16_t buf_idx;
        uint8_t retry_cnt;

        void* cookie;
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie);

        constexpr explicit I2cJob(I2cJobType type, uint16_t addr, bool addr_10bit,
            std::span<uint8_t> txbuf, std::span<uint8_t> rxbuf, void *cookie,
            void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie))
            noexcept
            : type(type), addr(addr), txbuf(txbuf), rxbuf(rxbuf), addr_10bit(addr_10bit),
            reading(type == I2cJobType::Read), buf_idx(0), retry_cnt(0), cookie(cookie),
            finished(finished) {}

        constexpr explicit I2cJob() noexcept : type(I2cJobType::Write), addr(0), txbuf(), rxbuf(),
            addr_10bit(false), reading(false), buf_idx(0), retry_cnt(0), cookie(nullptr),
            finished(nullptr) {}
    };

    inline void start_job(uint16_t addr, bool addr_10bit, bool rx, uint16_t txrx_bytes) noexcept
    {
        // set the required interrupt-flags
        usci.reg().ie.set(
            uscibregs::ifg::nackifg.value(1) +                         // set NACK int.
            uscibregs::ifg::alifg.value(1) +                           // set arbitration lost int.
            uscibregs::ifg::cltoifg.value(1) +                         // set clocklow-timeout int.
            uscibregs::ifg::txifg0.value(static_cast<uint16_t>(!rx)) + // set tx-reg empty int.
            uscibregs::ifg::rxifg0.value(static_cast<uint16_t>(rx))    // set rx-reg empty int.
        );

        // set slave-address
        usci.reg().i2csa.set(addr);

        // number of bytes that will be written AND read
        usci.reg().tbcnt.set(txrx_bytes);

        // setup the job-config
        usci.reg().ctlw0.modify(
            // set addressing-mode (7- or 10-bit)
            uscibregs::ctlw0::sla10.value(static_cast<uint16_t>(addr_10bit)) +
            // set transmitter / receiver mode
            uscibregs::ctlw0::tr.value(static_cast<uint16_t>(!rx)) +
            // send out start-condition
            uscibregs::ctlw0::txstt.value(1)
        );
    }

    void handle_interrupt() noexcept;

    UsciB& usci;
    I2cSpeed speed;
    bool initialized;
    std::atomic<bool> transmitting;
    Fifo<I2cJob, 16> jobfifo;
};

