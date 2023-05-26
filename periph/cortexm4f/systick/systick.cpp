// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#include <cstdint>

#include "systick.h"
#include "systick_regs.h"
#include "helpers.h"
#include "register.h"

constexpr uint32_t DIVIDE_TO_1MS = 1000;

void Systick::start(uint32_t clk) noexcept
{
    // we want an interrupt every 1ms
    reg().strvr.set(systickregs::strvr::reload.value(clk / DIVIDE_TO_1MS));

    // enable interrupt and enbale timer
    reg().stcsr.modify(systickregs::stcsr::tickint.value(1) + systickregs::stcsr::enable.value(1));
}

void Systick::stop() noexcept
{
    reg().stcsr.modify(systickregs::stcsr::tickint.value(0) + systickregs::stcsr::enable.value(0));
}

uint64_t Systick::uptime_ms() const noexcept
{
    return up;
}

void Systick::handle_interrupt() noexcept
{
    // since C++ 20 the ++ and += operator were deprecated for volatile operations
    up = up + 1;
}
