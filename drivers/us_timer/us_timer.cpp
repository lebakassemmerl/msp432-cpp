// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#include "us_timer.h"

void UsTimer::start() noexcept
{
    timer.init(false, nullptr, nullptr);
    timer.set_frequency(1'000'000, clock);
    timer.start();
}

uint32_t UsTimer::timestamp_diff_us(uint32_t old_ts, uint32_t new_ts) noexcept
{
    static constexpr uint32_t MAX_U32 = ~static_cast<uint32_t>(0);

    if (old_ts >= new_ts)
        return old_ts - new_ts;
    else
        return MAX_U32 - new_ts + old_ts;
}

void UsTimer::delay_us(uint16_t us) const noexcept
{
    uint32_t now = timer.get_load_register()
    volatile uint32_t until = now - reinterpret_cast<uint32_t>(us);

    if (now < until)
        while (time.get_load_register() < now);

    while (time.get_load_register() > until);
}
