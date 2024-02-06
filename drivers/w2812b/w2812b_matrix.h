// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "err.h"
#include "spi_master.h"
#include "w2812b.h"

template<uint16_t WIDTH, uint16_t HEIGHT>
class W2812BMatrix {
public:
    constexpr explicit W2812BMatrix(SpiMaster& spi) : matrix(spi) {}

    Err draw_pixel(uint16_t x, uint16_t y, Rgb col) noexcept
    {
        if ((x > (WIDTH - 1)) || (y > (HEIGHT - 1)))
            return Err::OutOfRange;

        if (!matrix.is_initialized())
            return Err::NotInitialized;

        draw_pixel_raw(x, y, col);
        return Err::Ok;
    }

    Err draw_line(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, Rgb col) noexcept
    {
        // currently we only support vertical, horizontal and 45deg lines since all other angles
        // would look rather weird

        uint16_t dx;
        uint16_t dy;

        if ((x0 > WIDTH) || (x1 > WIDTH) || (y0 > HEIGHT) || (y1 > HEIGHT))
            return Err::OutOfRange;

        if ((x1 <= x0) || (y1 <= y0))
            return Err::NotOk;

        if (!matrix.is_initialized())
            return Err::NotInitialized;

        dx = x1 - x0;
        dy = y1 - y0;

        if (dy == 0) {
            // horizontal line
            for (uint16_t i = x0; i <= x1; i++)
                draw_pixel_raw(i, y0, col);
        } else if (dx == 0) {
            // veritcal line
            for (uint16_t i = y0; i <= y1; i++)
                draw_pixel_raw(x0, i, col);
        } else if (dx == dy) {
            // diagonal line
            for (uint16_t x = x0, y = y0; x <= x1; x++, y++)
                draw_pixel_raw(x, y, col);
        } else {
            return Err::NotSupported;
        }
    }

    Err draw_rectangle(uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1, Rgb col) noexcept
    {
        if ((x0 > WIDTH) || (x1 > WIDTH) || (y0 > HEIGHT) || (y1 > HEIGHT))
            return Err::OutOfRange;

        if ((x1 <= x0) || (y1 <= y0))
            return Err::NotOk;

        if (!matrix.is_initialized())
            return Err::NotInitialized;

        for (uint16_t i = x0; i <= x1; i++) {
            draw_pixel_raw(i, y0, col); // top line
            draw_pixel_raw(i, y1, col); // bottom line
        }

        for (uint16_t i = y0 + 1; i < y1; i++) {
            draw_pixel_raw(x0, i, col); // left vertical line
            draw_pixel_raw(x1, i, col); // right vertical line
        }
    }

    Err init() noexcept
    {
        return matrix.init();
    }
private:
    inline void draw_pixel_raw(uint16_t x, uint16_t y, Rgb col) noexcept
    {
        if (y & 0x01) {
            // LEDs are concatenated from left to right
            matrix.set_color(y * WIDTH + x, col);
        } else {
            // LEDs are concatenated from right to left
            matrix.set_color(y * WIDTH + WIDTH - x - 1, col);
        }
    }

    W2812B<WIDTH * HEIGHT> matrix;
};
