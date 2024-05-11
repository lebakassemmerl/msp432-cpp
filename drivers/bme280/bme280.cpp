// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#include <array>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <span>

#include "bme280.h"
#include "err.h"
#include "msp432.h"

#define I16_TO_I32(x0, x1) \
    static_cast<int32_t>(static_cast<int16_t>(( \
        static_cast<int16_t>(x1) << 8) | static_cast<int16_t>(x0)))

#define I16_TO_I64(x0, x1) \
    static_cast<int64_t>(static_cast<int16_t>(( \
        static_cast<int16_t>(x1) << 8) | static_cast<int16_t>(x0)))

constexpr uint64_t UPDATE_TIME_MS = 1000;

constexpr std::array<uint8_t, 1> REG_CALIB1 = {0x88};
constexpr std::array<uint8_t, 1> REG_ID = {0xD0};
constexpr std::array<uint8_t, 1> REG_RESET = {0xE0};
constexpr std::array<uint8_t, 1> REG_CALIB2 = {0xE1};
constexpr std::array<uint8_t, 1> REG_CTRL = {0xF2};
constexpr std::array<uint8_t, 1> REG_RAW = {0xF7};

// normal mode, no filter, 1000ms standby time
constexpr std::array<uint8_t, 5> CMD_CONFIG = {REG_CTRL[0], 0x00, 0x00, 0x03, 0xA0};
constexpr std::array<uint8_t, 2> CMD_RESET = {REG_RESET[0], 0xB6};

void Bme280::Calibration::init_part1(std::span<uint8_t> data) noexcept
{
    temp[0] = static_cast<int32_t>(
        (static_cast<uint16_t>(data[1]) << 8) | static_cast<uint16_t>(data[0]));
    temp[1] = I16_TO_I32(data[2], data[3]);
    temp[2] = I16_TO_I32(data[4], data[5]);

    press[0] = static_cast<int64_t>(
        (static_cast<uint16_t>(data[7]) << 8) | static_cast<uint16_t>(data[6]));
    press[1] = I16_TO_I64(data[ 8], data[ 9]);
    press[2] = I16_TO_I64(data[10], data[11]);
    press[3] = I16_TO_I64(data[12], data[13]) << 35;
    press[4] = I16_TO_I64(data[14], data[15]);
    press[5] = I16_TO_I64(data[16], data[17]);
    press[6] = I16_TO_I64(data[18], data[19]) << 4;
    press[7] = I16_TO_I64(data[20], data[21]);
    press[8] = I16_TO_I64(data[22], data[23]);

    humid[0] = static_cast<int32_t>(data[25]);
}

void Bme280::Calibration::init_part2(std::span<uint8_t> data) noexcept
{
    humid[1] = I16_TO_I32(data[0], data[1]) + 8192;
    humid[2] = static_cast<int32_t>(data[3]);
    humid[3] = static_cast<int32_t>(static_cast<int16_t>((
        static_cast<int16_t>(data[4]) << 4) | (static_cast<int16_t>(data[5]) & 0x0F))) << 20;
    humid[4] = static_cast<int32_t>(static_cast<int16_t>((
            static_cast<int16_t>(data[6]) << 4) | (static_cast<int16_t>(data[5]) >> 4)));
    humid[5] = static_cast<int32_t>(data[7]);
}

void Bme280::i2c_cb(I2cJobType t, I2cErr err, std::span<uint8_t> rxbuf, void *cookie) noexcept
{
    // since we only set a flag, simply omit redirecting the callback to a member function
    Bme280* bme = reinterpret_cast<Bme280*>(cookie);
    bme->wait_for_cb.store(false, std::memory_order_relaxed);
}

void Bme280::periodic() noexcept
{
    int32_t var1;
    int32_t var2;
    uint64_t now;
    Err ret;

    if (state == Bme280::State::Initializing) {
        init();
        return;
    }

    if (!wait_for_cb.load(std::memory_order_relaxed)) {
        // we got data -> process it
        wait_for_cb.store(true, std::memory_order_relaxed);

        temp_raw = static_cast<int32_t>((static_cast<int16_t>(rxbuf[8]) << 12) |
            (static_cast<int16_t>(rxbuf[9]) << 4) | (static_cast<int16_t>(rxbuf[10] >> 4)));
        press_raw = static_cast<int32_t>((static_cast<int16_t>(rxbuf[5]) << 12) |
            (static_cast<int16_t>(rxbuf[6]) << 4) | (static_cast<int16_t>(rxbuf[7] >> 4)));
        humid_raw = static_cast<int32_t>(
            (static_cast<uint16_t>(rxbuf[11]) << 8) | static_cast<uint16_t>(rxbuf[12]));

        var1 = (((temp_raw >> 3) - (calib.temp[0] << 1)) * calib.temp[1]) >> 1;
        var2 = (((((temp_raw >> 4) - calib.temp[0]) * ((temp_raw >> 4) - calib.temp[0])) 
            >> 12) * calib.temp[2]) >> 14;

        calib.t_fine = var1 + var2;
    }

    now = Msp432::instance().cortexm4f().systick().uptime_ms();
    if ((now - last_sample) < UPDATE_TIME_MS)
        return;

    ret = i2c.write_read(
        i2c_addr, std::span{REG_RAW}, std::span{rxbuf.data(), 13}, this, Bme280::i2c_cb);

    if (ret != Err::Ok)
        return;

    // command was successfully queued -> return and wait for the data
    last_sample = now;
    return;
}

void Bme280::init() noexcept
{
    Err err;

    if (wait_for_cb.load(std::memory_order_relaxed))
        return;

    wait_for_cb.store(true, std::memory_order_relaxed);

    switch (init_state) {
    case InitState::QueueDevice: 
        err = i2c.write_read(
            i2c_addr, std::span{REG_ID}, std::span{rxbuf.data(), 1}, this, Bme280::i2c_cb);

        if (err == Err::Ok)
            init_state = InitState::CheckDevice;

        break;

    case InitState::CheckDevice:
        if (rxbuf[0] != 0x60) {
            state = State::WrongDevice;
            break;
        }
        // fall through

    case InitState::Reset:
        err = i2c.write(i2c_addr, std::span{CMD_RESET}, this, Bme280::i2c_cb);
        if (err == Err::Ok)
                init_state = InitState::WriteConfig;

        break;

    case InitState::WriteConfig:
        err = i2c.write(i2c_addr, std::span{CMD_CONFIG}, this, Bme280::i2c_cb);
        if (err == Err::Ok)
                init_state = InitState::ReadCalib1;

        break;

    case InitState::ReadCalib1:
        err = i2c.write_read(
            i2c_addr, std::span{REG_CALIB1}, std::span{rxbuf}, this, Bme280::i2c_cb);

        if (err == Err::Ok)
            init_state = InitState::ReadCalib2;

        break;

    case InitState::ReadCalib2:
        // we read out successfully the calib1 values -> parse them now
        calib.init_part1(rxbuf);

        err = i2c.write_read(
            i2c_addr, std::span{REG_CALIB2}, std::span{rxbuf.data(), 16}, this,
            Bme280::i2c_cb);

        if (err == Err::Ok)
            init_state = InitState::Finished;

        break;

    case InitState::Finished:
        // we read out successfully the calib2 values -> parse them and finish the init-sequence
        calib.init_part2(rxbuf);
        state = State::Ready;
        break;

    default:
        break;
    }
}

int32_t Bme280::temperature() const noexcept
{
    if (state != State::Ready)
        return -300'000;

    return (((calib.t_fine * 5 + 128) >> 8) * 10);
}


