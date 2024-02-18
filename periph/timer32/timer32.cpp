// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <cstddef>

#include "cs.h"
#include "err.h"
#include "timer32.h"
#include "timer32_regs.h"

Err 
Timer32::init(bool use_interrupt, void* cookie, void (*callback)(void *cookie) noexcept) noexcept
{
    if (is_initialized())
        return Err::Ok;

    if (use_interrupt) {
        if(!callback)
            return Err::NullPtr;

        status |= STATUS_USE_INTERRUPT;
    }

    this->cb = callback;
    this->cookie = cookie;

    reg().control.modify(
        timer32regs::control::mode.value(1)         // periodic mode
        + timer32regs::control::ie.value(0)         // disable interrupt
        + timer32regs::control::prescale.value(0)   // divide clock by 1
        + timer32regs::control::size.value(1)       // 32bit mode
        + timer32regs::control::oneshot.value(0)    // wrapping mode
    );

    status |= STATUS_INITIALIZED;
    return Err::Ok;
}

Err Timer32::set_frequency(uint32_t freq_hz, const Cs& cs) noexcept
{
    uint32_t tmp;
    uint32_t mclk;

    if (is_running())
        return Err::Busy;

    mclk = cs.m_clk();
    if ((freq_hz == 0) || (freq_hz > mclk))
        return Err::OutOfRange;

    tmp = mclk / freq_hz;
    freq = mclk / tmp;

    reg().load.set(tmp); // set the reload value

    return Err::Ok;
}

Err Timer32::start() noexcept
{
    if ((!is_initialized()) || (freq == 0) || (cb == nullptr))
        return Err::NotInitialized;

    reg().control.modify(
        timer32regs::control::ie.value((status & STATUS_USE_INTERRUPT) > 0) // enable interrupt
        + timer32regs::control::enable.value(1)                             // enable / start timer
    );

    status |= STATUS_RUNNING;
    return Err::Ok;
}

void Timer32::stop() noexcept
{
    if (!is_running())
        return;

    reg().control.modify(
        timer32regs::control::ie.value(0)       // disable interrupt
        + timer32regs::control::enable.value(0) // disable / stop timer
    );

    reg().intclr.set(1); // write any value to this register to clear an eventual interrupt

    status &= ~STATUS_RUNNING;
}

void Timer32::handle_interrupt() noexcept
{
    reg().intclr.set(1); // clear interrupt by writing any value to this register;
    cb(cookie);
}
