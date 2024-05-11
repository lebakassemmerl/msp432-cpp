// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <array>
#include <atomic>
#include <cstdint>

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
        : i2c(i2c), i2c_addr(static_cast<uint16_t>(addr)), state(State::Initializing),
        init_state(InitState::QueueDevice), last_sample(0), wait_for_cb(false), rxbuf(), calib(),
        temp_raw(0), humid_raw(0), press_raw(0) {}

    void periodic() noexcept;

    // returns the temperature in centi-degrees -> e.g. 2375 equal 23.75degC
    int32_t temperature() const noexcept;

private:
    enum class InitState : uint8_t {
        QueueDevice,
        CheckDevice,
        Reset,
        ReadCalib1,
        ReadCalib2,
        WriteConfig,
        Finished,
    };

    class Calibration {
    public:
        std::array<int32_t, 3> temp;
        std::array<int32_t, 6> humid;
        std::array<int64_t, 9> press;
        int32_t t_fine;

        constexpr explicit Calibration() noexcept : temp(), humid(), press(), t_fine(0) {}

        void init_part1(std::span<uint8_t> data) noexcept;
        void init_part2(std::span<uint8_t> data) noexcept;
    };

    static void i2c_cb(I2cJobType t, I2cErr err, std::span<uint8_t> rxbuf, void *cookie) noexcept;

    void init() noexcept;

    I2cMaster& i2c;
    uint16_t i2c_addr;
    Bme280::State state;
    Bme280::InitState init_state;
    uint64_t last_sample;
    std::atomic<bool> wait_for_cb;
    std::array<uint8_t, 26> rxbuf;

    Calibration calib;
    int32_t temp_raw;
    int32_t humid_raw;
    int32_t press_raw;
};

