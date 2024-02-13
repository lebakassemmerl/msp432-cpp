// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <array>
#include <cstdint>
#include <span>

#include "err.h"
#include "pin.h"
#include "spi.h"
#include "spi_master.h"

class Lt7920 {
public:
    static constexpr size_t PIX_WIDTH = 128;
    static constexpr size_t PIX_HEIGHT = 64;

    constexpr explicit Lt7920(SpiMaster& spi, Pin& cs) noexcept
        : fb(), initialized(false), fb_idx(0), spi(spi), cs(cs) {}

    Err init() noexcept;
    Err display_on() noexcept;
    Err display_off() noexcept;

    Err set_pixel(uint8_t x, uint8_t y, bool val) noexcept;
private:
    static void redirect_cmd_cb(
        SpiTransferType type,
        std::span<uint8_t> txbuf,
        std::span<uint8_t> rxbuf,
        void* context) noexcept;

    static void redirect_write_cb(
        SpiTransferType type,
        std::span<uint8_t> txbuf,
        std::span<uint8_t> rxbuf,
        void* context) noexcept;

    void write_cb() noexcept;
    void cmd_cb() noexcept;
    inline void update_fb_idx() noexcept { fb_idx = (fb_idx + 3) % fb.size(); }

    std::array<uint8_t, 3 * PIX_WIDTH * PIX_HEIGHT / 8> fb;
    bool initialized;
    size_t fb_idx;

    SpiMaster& spi;
    Pin& cs;
};
