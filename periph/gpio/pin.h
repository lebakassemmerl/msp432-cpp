// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "gpio_regs.h"

enum class PullMode {
    PullUp,
    PullDown,
    None
};

enum class PinNr : uint8_t {
    P07_0, P07_1, P07_2, P07_3, P07_4, P07_5, P07_6, P07_7,
    P08_0, P08_1, P08_2, P08_3, P08_4, P08_5, P08_6, P08_7,
    P09_0, P09_1, P09_2, P09_3, P09_4, P09_5, P09_6, P09_7,
    P10_0, P10_1, P10_2, P10_3, P10_4, P10_5, P10_6, P10_7,
    PJ_0,  PJ_1,  PJ_2,  PJ_3,  PJ_4,  PJ_5,  PJ_6,  PJ_7,
};

class Pin {
public:
    constexpr ~Pin() noexcept {};

    void make_output() const noexcept;
    void make_input() const noexcept;
    bool is_output() const noexcept;

    void set_high() const noexcept;
    void set_low() const noexcept;
    void toggle() const noexcept;

    bool read() const noexcept;
    void set_pull_mode(PullMode mode) const noexcept;
    PullMode get_pull_mode() const noexcept;

    void enable_primary_function() const noexcept;
    void enable_secondary_function() const noexcept;
    void enable_tertiary_function() const noexcept;

    friend class GpioPins;
protected:
    static constexpr uint8_t PINS_PER_PORT = 8;

    enum class PinFunction {
        Gpio,
        Primary,
        Secondary,
        Tertiary,
    };

    constexpr explicit Pin(PinNr nr) noexcept
        : Pin(static_cast<uint8_t>(nr) % PINS_PER_PORT,
        (static_cast<uint8_t>(nr) / PINS_PER_PORT) % 2,
        3 + ((static_cast<uint8_t>(nr) / PINS_PER_PORT) / 2)) {}

    constexpr explicit Pin(uint8_t pin_nr, uint8_t reg_idx, uint8_t reg_base) noexcept
        : pin_nr(pin_nr), reg_idx(reg_idx), reg_base(reg_base) {}

    // helper function to convert the raw address to a reference since a reinterpret_cast is not
    // allowed in constexpr functions
    inline GpioRegisters& reg() const noexcept
    {
        return *reinterpret_cast<GpioRegisters*>(GPIO_BASES[reg_base]);
    }

    inline void set_pin_function(PinFunction func) const noexcept
    {
        uint8_t sel0 = reg().sel0[reg_idx].get();
        uint8_t sel1 = reg().sel1[reg_idx].get();

        switch (func) {
        case PinFunction::Gpio:
            sel0 &= ~(1 << pin_nr);
            sel1 &= ~(1 << pin_nr);
            break;

        case PinFunction::Primary:
            sel0 |= 1 << pin_nr;
            sel1 &= ~(1 << pin_nr);
            break;

        case PinFunction::Secondary:
            sel0 &= ~(1 << pin_nr);
            sel1 |= 1 << pin_nr;
            break;

        case PinFunction::Tertiary:
            sel0 |= 1 << pin_nr;
            sel1 |= 1 << pin_nr;
            break;
        }

        reg().sel0[reg_idx].set(sel0);
        reg().sel1[reg_idx].set(sel1);
    }

    const uint8_t pin_nr;
    const uint8_t reg_idx;
    const uint8_t reg_base;
};
