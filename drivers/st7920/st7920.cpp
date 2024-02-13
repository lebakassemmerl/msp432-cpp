// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#include <array>
#include <cstdint>
#include <span>

#include "err.h"
#include "nt7108c.h"

constexpr uint8_t TRANSFER_DATA = 0xFA;
constexpr uint8_t TRANSFER_CMD = 0xF8;

constexpr std::array<uint8_t, 3> generate_cmd(uint8_t cmd)
{
    // for some weird reason we need these temporary variables to a void a narrowing warning..
    uint8_t cmd_high = cmd & 0xF0u;
    uint8_t cmd_low = (cmd & 0x0Fu) << 4;
    return std::array<uint8_t, 3>{{TRANSFER_CMD, cmd_high, cmd_low}};
}

constexpr std::array<uint8_t, 3> CMD_CLEAR_SCREEN = generate_cmd(0x01);
constexpr std::array<uint8_t, 3> CMD_HOME = generate_cmd(0x02);
constexpr std::array<uint8_t, 3> CMD_ADDR_INC = generate_cmd(0x06);
constexpr std::array<uint8_t, 3> CMD_DISP_OFF = generate_cmd(0x08);
constexpr std::array<uint8_t, 3> CMD_DISP_ON = generate_cmd(0x0C);
constexpr std::array<uint8_t, 3> CMD_CURSOR_BLINK = generate_cmd(0x0F);
constexpr std::array<uint8_t, 3> CMD_BASIC_MODE = generate_cmd(0x30);
constexpr std::array<uint8_t, 3> CMD_SET_ADDR_0 = generate_cmd(0x80);

void Lt7920::redirect_cmd_cb(
    SpiTransferType type,
    std::span<uint8_t> txbuf,
    std::span<uint8_t> rxbuf,
    void* context) noexcept
{
    Lt7920* instance = reinterpret_cast<Lt7920*>(context);
    instance->cmd_cb();
}

void Lt7920::redirect_write_cb(
    SpiTransferType type,
    std::span<uint8_t> txbuf,
    std::span<uint8_t> rxbuf,
    void* context) noexcept
{
    Lt7920* instance = reinterpret_cast<Lt7920*>(context);
    instance->write_cb();
}

void Lt7920::write_cb() noexcept
{
    // spi.write(std::span{CMD_HOME.data(), CMD_HOME.size()}, &cs, nullptr, nullptr);
    spi.write(std::span{&fb[fb_idx], 3}, &cs, this, redirect_write_cb);
    update_fb_idx();
}

void Lt7920::cmd_cb() noexcept
{
    if (!initialized) {
        initialized = true;

        // start writing the framebuffer to the display
        // spi.write(std::span{CMD_HOME.data(), CMD_HOME.size()}, &cs, nullptr, nullptr);
        spi.write(std::span{&fb[fb_idx], 3}, &cs, this, redirect_write_cb);
        update_fb_idx();
    }
}

Err Lt7920::init() noexcept
{
    if (spi.get_actual_freq_hz() > 2'500'000)
        return Err::OutOfRange;

    cs.make_output();
    cs.set_low();

    for (size_t i = 0; i < fb.size(); i+= 3)
        fb[i] = TRANSFER_DATA;

    spi.write(std::span{CMD_BASIC_MODE.data(), CMD_BASIC_MODE.size()}, &cs, nullptr, nullptr);
    spi.write(std::span{CMD_BASIC_MODE.data(), CMD_BASIC_MODE.size()}, &cs, nullptr, nullptr);
    // spi.write(std::span{CMD_CLEAR_SCREEN.data(), CMD_CLEAR_SCREEN.size()}, &cs, nullptr, nullptr);
    spi.write(std::span{CMD_ADDR_INC.data(), CMD_ADDR_INC.size()}, &cs, nullptr, nullptr);
    spi.write(std::span{CMD_DISP_ON.data(), CMD_DISP_ON.size()}, &cs, nullptr, nullptr);
    spi.write(std::span{CMD_SET_ADDR_0.data(), CMD_SET_ADDR_0.size()}, &cs, this, redirect_cmd_cb);

    return Err::Ok;
}

Err Lt7920::set_pixel(uint8_t x, uint8_t y, bool val) noexcept
{
    size_t idx;
    if ((x >= PIX_WIDTH) || (y >= PIX_HEIGHT))
        return Err::OutOfRange;

    // calculate the row
    idx = 1 + static_cast<size_t>(y) * PIX_WIDTH / 8 * 3;
    idx += static_cast<size_t>(x >> 3) * 3;

    x &= 0x07;
    if (x < 4) {
        idx += 1;
        x += 4;
    }

    if (val)
        fb[idx] |= 1 << x;
    else
        fb[idx] &= ~static_cast<uint8_t>(1 << x);

    return Err::Ok;
}
