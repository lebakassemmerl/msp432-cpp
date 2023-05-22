// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include "gpio.h"
#include "led.h"

void Led::init() noexcept
{
    pin.make_output();
    off();
}

void Led::on() noexcept
{
    if (active_low)
        pin.set_low();
    else
        pin.set_high();
}

void Led::off() noexcept
{
    if (active_low)
        pin.set_high();
    else
        pin.set_low();
}

void Led::toggle() noexcept { pin.toggle(); }
