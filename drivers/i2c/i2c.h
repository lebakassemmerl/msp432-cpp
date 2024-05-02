// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>
#include <span>

#include "err.h"
#include "fifo.h"
#include "usci.h"
#include "uscib_regs.h"

enum class I2cErr : uint8_t  {
    Ok,
    Nack,
    ArbitrationLost,
};

enum class I2cJobType {
    Write,
    Read,
    WriteRead,
};

class I2cMaster {
public:
    constexpr explicit I2cMaster(UsciB& usci, uint32_t i2c_freq)
        : usci(usci), freq(i2c_freq), initialized(false), jobfifo() {}

    Err init() noexcept;

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
        I2cJobType type;
        uint16_t addr;
        std::span<uint8_t> txbuf;
        std::span<uint8_t> rxbuf;
        bool addr_10bit;

        void* cookie;
        void (*finished)(I2cJobType type, I2cErr err, std::span<uint8_t> rxbuf, void* cookie);
    };

    UsciB& usci;
    uint32_t freq;
    bool initialized;
    Fifo<I2cJob, 16> jobfifo;
};

