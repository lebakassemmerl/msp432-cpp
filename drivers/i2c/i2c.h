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
        std::span<const uint8_t> txbuf;
        std::span<uint8_t> rxbuf;
        bool addr_10bit;

        uint16_t buf_idx;
        uint8_t retry_cnt;

        void* cookie;
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie);

        constexpr explicit I2cJob(I2cJobType type, uint16_t addr, bool addr_10bit,
            std::span<const uint8_t> txbuf, std::span<uint8_t> rxbuf, void *cookie,
            void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie))
            noexcept
            : type(type), addr(addr), txbuf(txbuf), rxbuf(rxbuf), addr_10bit(addr_10bit),
            buf_idx(0), retry_cnt(0), cookie(cookie), finished(finished) {}

        constexpr explicit I2cJob() noexcept : type(I2cJobType::Write), addr(0), txbuf(), rxbuf(),
            addr_10bit(false), buf_idx(0), retry_cnt(0), cookie(nullptr), finished(nullptr) {}
    };

    void start_job(uint16_t addr, bool addr_10bit, bool rx, uint16_t txrx_bytes) noexcept;
    void handle_interrupt() noexcept;

    UsciB& usci;
    I2cSpeed speed;
    bool initialized;
    std::atomic<bool> transmitting;
    Fifo<I2cJob, 16> jobfifo;
};

