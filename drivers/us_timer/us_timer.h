// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include "cs.h"
#include "timer32.h"

class UsTimer {
public:
    constexpr explicit UsTimer(Timer32& timer, Cs& clock) noexcept : timer(timer), clock(clock) {}

    void start() noexcept;

    inline void stop() noexcept
    {
        timer.stop();
    }

    void delay_us(uint16_t us) const noexcept;

    inline uint32_t timestamp_us() noexcept
    {
        return timer.get_load_register();
    }

    uint32_t timestamp_diff_us(uint32_t old_ts, uint32_t new_ts) noexcept;
    inline uint32_t timestamp_diff_us(uint32_t old_ts) noexcept 
    {
        return timestamp_diff_us(old_ts, timestamp_us());
    }
private:
    Timer32& timer;
    Cs& clock;
};
