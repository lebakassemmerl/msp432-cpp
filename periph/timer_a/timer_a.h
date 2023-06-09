// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "cs.h"
#include "err.h"
#include "helpers.h"
#include "timer_a_regs.h"

class TimerA {
public:
    enum class CountMode : uint16_t {
        Up = 1,
        Continuous = 2,
        UpDown = 3,
    };

    enum class TimerMode : uint8_t {
        Off,
        Count,
        Capture,
        Pwm,
    };

    enum class OutputMode : uint16_t {
        Output = 0,
        Set = 1,
        ToggleReset = 2,
        SetReset = 3,
        Toggle = 4,
        Reset = 5,
        ToggleSet = 6,
        ResetSet = 7,
    };

    constexpr explicit TimerA(size_t reg_base) noexcept
        : mode(TimerMode::Off), reg_base(reg_base) {}

    Err set_mode(TimerMode mode) noexcept;
    Err setup_pwm(const Cs& cs, size_t freq_hz, uint8_t duty_cycle) noexcept;
    Err update_dutycycle(uint8_t duty_cycle) noexcept;

    Err start() noexcept;
    Err stop() noexcept;

private:
    inline TimerARegisters& reg() noexcept
    {
        return *reinterpret_cast<TimerARegisters*>(reg_base);
    }

    TimerMode mode;
    const size_t reg_base;
};
