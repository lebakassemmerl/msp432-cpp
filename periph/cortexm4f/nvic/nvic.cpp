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
    uint64_t iabr = reg().iabr1.get();
    iabr <<= 32;
    iabr |= reg().iabr0.get();

    return iabr;
}

void Nvic::clear_all_pending() noexcept
{
    reg().icpr0.set(~0);
    reg().icpr1.set(~0);
}

void Nvic::enable_all_interrupts() noexcept
{
    reg().iser0.set(~0);
    reg().iser1.set(~0);
}

void Nvic::disable_all_interrupts() noexcept
{
    reg().icer0.set(~0);
    reg().icer1.set(~0);
}
