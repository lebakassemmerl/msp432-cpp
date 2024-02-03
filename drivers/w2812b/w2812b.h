// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 *
 * This driver implements the W2812B protocol used in these standard LED-stripes where an arbitrary
 * number of RGB-LEDs are cascaded. Most implementations implement this protocol by bit-banging
 * which of course is rather inefficient. This driver tries to use an SPI-bus with a certain timing
 * in combination with DMA to provide a fast and efficient implementation.
 *
 * In order to get this driver working one has to provide a reference to an SPI-module which is
 * configured to a clock-frequency of exactly 6MHz. If we do the math, 6MHz seems the optimum
 * frequency because we "only" need to transfer only 1 byte per bit and still comply to the given
 * timings. If the provided SPI-module is setup to a different frequency than 6MHz, the
 * init-function will fail and this driver won't work.
 */

#pragma once

#include <array>
#include <atomic>
#include <bit>
#include <cstdint>
#include <cstddef>
#include <span>

#include "err.h"
#include "helpers.h"
#include "libc.h"
#include "spi.h"
#include "spi_master.h"

struct Rgb {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    uint32_t to_raw() const noexcept
    {
        uint32_t raw = (static_cast<uint32_t>(b)) |
                       (static_cast<uint32_t>(r) << 8) |
                       (static_cast<uint32_t>(g) << 16);

        return raw;
    }
};

template<size_t N>
class W2812B {
public:
    constexpr explicit W2812B(SpiMaster& spi)
        : spi(spi), fb(), transmitting(false), initialized(false) {}

    Err init() noexcept
    {
        uint32_t spi_freq = spi.get_actual_freq_hz();
        if ((spi_freq >= SPI_FREQ_HZ_MIN) && (spi_freq <= SPI_FREQ_HZ_MAX)) {
            initialized = true;
            return Err::Ok;
        } else {
            return Err::NotSupported;
        }
    }

    Err set_color(size_t led_idx, Rgb col)
    {
        uint32_t raw;

        if (!initialized)
            return Err::NotInitialized;

        if (led_idx >= N)
            return Err::OutOfRange;

        if (transmitting.load(std::memory_order::relaxed))
            return Err::Busy;

        raw = col.to_raw();
        for (unsigned i = 0; i < WORDS_PER_LED; i++) {
            // the highest green bit has to be transmitted first, thus the reverse array-access
            fb[led_idx * WORDS_PER_LED + (WORDS_PER_LED - i - 1)] = LUT[raw & 0x0F];
            raw >>= 4;
        }

        return Err::Ok;
    }

    Err set_color_for_all_leds(Rgb col) noexcept
    {
        uint32_t raw_color[WORDS_PER_LED];
        uint32_t raw;

        if (!initialized)
            return Err::NotInitialized;

        if (transmitting.load(std::memory_order::relaxed))
            return Err::Busy;

        raw = col.to_raw();
        for (size_t i = 0; i < WORDS_PER_LED; i++) {
            // the highest green bit has to be transmitted first, thus the reverse array-access
            raw_color[WORDS_PER_LED - i - 1] = LUT[raw & 0x0F];
            raw >>= 4;
        }

        for (size_t i = 0; i < N; i++)
            libc::memcpy(&fb[i * WORDS_PER_LED], raw_color, sizeof(raw_color));

        return Err::Ok;
    }

    Err switch_off_all_leds() noexcept
    {
        if (!initialized)
            return Err::NotInitialized;

        if (transmitting.load(std::memory_order::relaxed))
            return Err::Busy;

        libc::memset(fb.data(), ZERO, N * 4);
        return Err::Ok;
    }

    Err refresh_leds() noexcept
    {
        if (!initialized)
            return Err::NotInitialized;

        if (transmitting.load(std::memory_order::relaxed))
            return Err::Busy;

        transmitting.store(true, std::memory_order::acquire);
        return spi.write(
            std::span<uint8_t>{reinterpret_cast<uint8_t*>(fb.data()), fb.size() * sizeof(fb[0])},
            this, redirect_spi_cb);
    }

private:
    // Minimum an maximum frequency to satisfy the required timing defined in the spec. For
    // calculation details look at the spec (https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)
    // and the provided python-script 'timing_calc.py'.
    static constexpr uint32_t SPI_FREQ_HZ_MIN = 5'600'000;
    static constexpr uint32_t SPI_FREQ_HZ_MAX = 7'000'000;

    static constexpr size_t WORDS_PER_LED = 24 / 4; // 24 bytes = 6 uint32
    static constexpr uint32_t ZERO = 0b11100000;
    static constexpr uint32_t ONE = 0b11111000;

    // we calculate a lookup-table in for encoding 4 bits into the framebuffer
    static constexpr std::array<uint32_t, 16> LUT = [] () {
        std::array<uint32_t, 16> ret{};

        for (unsigned var = 0; var < 16; var++) {
            uint32_t tmp = 0;
            if constexpr (std::endian::native == std::endian::little) {
                for (unsigned i = 0, testbit = 1 << 3; i < 4; i++, testbit >>= 1) {
                    if (var & testbit) {
                        tmp |= ONE << (i * 8);
                    } else {
                        tmp |= ZERO << (i * 8);
                    }
                    ret[var] = tmp;
                }
            } else {
                for (unsigned i = 0, testbit = 1; i < 4; i++, testbit <<= 1) {
                    if (var & testbit) {
                        tmp |= ONE << (i * 8);
                    } else {
                        tmp |= ZERO << (i * 8);
                    }
                    ret[var] = tmp;
                }
            }
        }
        return ret;
    }();

    static void redirect_spi_cb(
        SpiTransferType type,
        std::span<uint8_t> txbuf,
        std::span<uint8_t> rxbuf,
        void* context) noexcept
    {
        W2812B<N>* instance = reinterpret_cast<W2812B<N>*>(context);
        instance->handle_spi_cb();
    }

    void handle_spi_cb() noexcept
    {
        transmitting.store(false, std::memory_order::release);
    }

    SpiMaster& spi; // the SPI-module has to be configured to 6MHz SCK, otherwise this won't work
    std::array<uint32_t, N * WORDS_PER_LED> fb;
    std::atomic<bool> transmitting;
    bool initialized;
};