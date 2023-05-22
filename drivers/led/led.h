// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include "gpio.h"

class Led {
public:
    constexpr explicit Led(Pin& led, bool active_low) noexcept : pin(led), active_low(active_low) {}

    void init() noexcept;
    void on() noexcept;
    void off() noexcept;
    void toggle() noexcept;
private:
    Pin& pin;
    bool active_low;
};
