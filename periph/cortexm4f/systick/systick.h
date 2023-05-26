// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "systick_regs.h"

class Systick {
public:
    Systick(const Systick&) = delete;
    Systick(const Systick&&) = delete;
    Systick& operator=(const Systick&) = delete;
    Systick& operator=(const Systick&&) = delete;
    constexpr ~Systick() noexcept {}

    void start(uint32_t clk) noexcept;
    void stop() noexcept;

    uint64_t uptime_ms() const noexcept;

    friend class CortexM4F;
    friend void systick_handler(void) noexcept;
private:
    constexpr explicit Systick() noexcept : reg_addr(SYSTICK_BASE), up(0) {}

    inline SystickRegisters& reg() const noexcept
    {
        return *reinterpret_cast<SystickRegisters*>(reg_addr);
    }

    void handle_interrupt() noexcept;

    const size_t reg_addr;
    uint64_t up;
};