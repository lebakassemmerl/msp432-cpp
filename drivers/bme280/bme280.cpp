// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#include <array>
#include <atomic>
#include <cstdint>
#include <span>

#include "bme280.h"
#include "err.h"

constexpr std::array<uint8_t, 1> REG_CALIB1 = {0x88};
constexpr std::array<uint8_t, 1> REG_ID = {0xD0};
constexpr std::array<uint8_t, 1> REG_RESET = {0xE0};
constexpr std::array<uint8_t, 1> REG_CALIB2 = {0xE1};
constexpr std::array<uint8_t, 1> REG_CTRL = {0xF2};
constexpr std::array<uint8_t, 1> REG_RAW = {0xF7};

constexpr std::array<uint8_t, 1> CMD_RESET = {0xB6};

void Bme280::i2c_cb(I2cJobType t, I2cErr err, std::span<uint8_t> rxbuf, void *cookie) noexcept
{
    // since we only set a flag, simply omit redirecting the callback to a member function
    Bme280* bme = reinterpret_cast<Bme280*>(cookie);
    bme->wait_for_cb.store(false, std::memory_order_relaxed);
}

void Bme280::periodic()
{
    if (state == Bme280::State::Initializing) {
        init();
        return;
    }


}

void Bme280::init()
{
    Err err;

    if (wait_for_cb.load(std::memory_order_relaxed))
        return;

    wait_for_cb.store(true, std::memory_order_relaxed);

    switch (init_state) {
    case Bme280::InitState::CheckDevice: 
        err = i2c.write_read(
            i2c_addr, std::span{REG_ID}, std::span{rxbuf.data(), 1}, this, Bme280::i2c_cb));

        if (err == Err::Ok)
            init_state = Reset;

        break;
    }
    };
}
