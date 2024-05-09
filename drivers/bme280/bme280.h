// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <array>
#include <atomic>
#include <cstdint>

#include "err.h"
#include "helpers.h"
#include "i2c.h"

class Bme280 {
public:
    enum class State : uint8_t {
        Initializing,
        WrongDevice,
        DeviceNotFound,
        Ready,
        Unknown,
    };

    constexpr explicit Bme280(I2cMaster& i2c, uint8_t addr) noexcept
        : i2c(i2c), i2c_addr(static_cast<uint16_t>(addr)), state(Initializing),
        init_state(CheckDevice), last_sample(0), wait_for_cb(false), rxbuf() {}

    void periodic() noexcept;

private:
    enum class InitState : uint8_t {
        CheckDevice,
        Reset,
        WriteConfig,
        ReadCalib1,
        ReadCalib2,
    };

    static void i2c_cb(I2cJobType t, I2cErr err, std::span<uint8_t> rxbuf, void *cookie) noexcept;

    I2cMaster& i2c;
    uint16_t i2c_addr;
    Bme280::State state;
    Bme280::InitState init_state;
    uint64_t last_sample;
    std::atomic<bool> wait_for_cb;
    std::array<uint8_t 26> rxbuf;
};

