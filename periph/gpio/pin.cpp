// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include "pin.h"

void Pin::enable_primary_function() const noexcept
{
    set_pin_function(Pin::PinFunction::Primary);
}
void Pin::enable_secondary_function() const noexcept
{
    set_pin_function(Pin::PinFunction::Secondary);
}
void Pin::enable_tertiary_function() const noexcept
{
    set_pin_function(Pin::PinFunction::Tertiary);
}

void Pin::make_output() const noexcept
{
    set_pin_function(Pin::PinFunction::Gpio);

    reg().dir[reg_idx].set(reg().dir[reg_idx].get()| 1 << pin_nr);

    set_low();
}

void Pin::make_input() const noexcept
{
    set_pin_function(Pin::PinFunction::Gpio);

    reg().dir[reg_idx].set(reg().dir[reg_idx].get() & ~(1 << pin_nr));
}

bool Pin::is_output() const noexcept
{
    return (reg().dir[reg_idx].get() & (1 << pin_nr)) > 0;
}

void Pin::set_high() const noexcept
{
    reg().out[reg_idx].set(reg().out[reg_idx].get() | (1 << pin_nr));
}

void Pin::set_low() const noexcept
{
    reg().out[reg_idx].set(reg().out[reg_idx].get() & ~(1 << pin_nr));
}

void Pin::toggle() const noexcept
{
    reg().out[reg_idx].set(reg().out[reg_idx].get() ^ (1 << pin_nr));
}

bool Pin::read() const noexcept
{
    return (reg().in[reg_idx].get() & (1 << pin_nr)) > 0;
}

void Pin::set_pull_mode(PullMode mode) const noexcept
{
    if (is_output())
        return;

    uint8_t ren = reg().ren[reg_idx].get();
    uint8_t out = reg().out[reg_idx].get();

    switch (mode) {
    case PullMode::PullUp:
        ren |= 1 << pin_nr;
        out |= 1 << pin_nr;
        break;

    case PullMode::PullDown:
        ren |= 1 << pin_nr;
        out &= ~(1 << pin_nr);
        break;

    case PullMode::None:
        ren &= ~(1 << pin_nr);
        break;
    }

    reg().ren[reg_idx].set(ren);
    reg().out[reg_idx].set(out);
}

PullMode Pin::get_pull_mode() const noexcept
{
    if (is_output())
        return PullMode::None;

    bool ren = (reg().ren[reg_idx].get() & (1 << pin_nr)) > 0;
    bool out = (reg().out[reg_idx].get() & (1 << pin_nr)) > 0;

    if (!ren) {
        return PullMode::None;
    } else {
        if (out)
            return PullMode::PullUp;
        else
            return PullMode::PullDown;
    }
}
