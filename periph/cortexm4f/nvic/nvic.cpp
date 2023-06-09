// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#include "helpers.h"
#include "register.h"

#include "nvic.h"

uint64_t Nvic::get_pending_interrupts() noexcept
{
    uint64_t iabr = reg().iabr[1].get();
    iabr <<= 32;
    iabr |= reg().iabr[0].get();

    return iabr;
}

void Nvic::clear_all_pending() noexcept
{
    reg().icpr[0].set(~0);
    reg().icpr[1].set(~0);
}

void Nvic::enable_all_interrupts() noexcept
{
    constexpr uint32_t UNUSED_1 = hlp::bit<uint32_t>(27);       // shared Timer32 interrupt
    constexpr uint32_t UNUSED_2 = hlp::mask<uint32_t>(31, 9);   // unused

    reg().iser[0].set(~UNUSED_1);
    reg().iser[1].set(~UNUSED_2);
}

void Nvic::disable_all_interrupts() noexcept
{
    reg().icer[0].set(~0);
    reg().icer[1].set(~0);
}

void Nvic::clear_pending(size_t idx) noexcept
{
    reg().icer[(idx >> 5) & 0x01].set(idx & 0x1F);
}
