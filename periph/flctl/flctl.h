// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "flctl_regs.h"

enum class WaitStates {
    _0 = 0, _1, _2, _3, _4, _5, _6, _7,
    _8, _9, _10, _11, _12, _13, _14, _15,
};

class FlCtl {
public:
    FlCtl(const FlCtl&) = delete;
    FlCtl(const FlCtl&&) = delete;
    FlCtl& operator=(const FlCtl&) = delete;
    FlCtl& operator=(const FlCtl&&) = delete;
    constexpr ~FlCtl() {};

    void set_waitstates(WaitStates ws) const noexcept;
    void set_buffering(bool enable) const noexcept;

    friend void init_platform(void); // from startup.cpp
    friend class Msp432;
private:
    constexpr explicit FlCtl() noexcept : reg_addr(FLCTL_BASE) {}

    inline FlCtlRegisters& reg() const noexcept
    {
        return *reinterpret_cast<FlCtlRegisters*>(reg_addr);
    }

    const size_t reg_addr;
};