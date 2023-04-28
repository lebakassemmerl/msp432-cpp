// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "wdt_regs.h"

class Wdt {
public:
    Wdt(const Wdt&) = delete;
    Wdt(const Wdt&&) = delete;
    Wdt& operator=(const Wdt&) = delete;
    Wdt& operator=(const Wdt&&) = delete;
    constexpr ~Wdt() noexcept {}

    void enable() const noexcept;
    void disable() const noexcept;
    void tickle() const noexcept;

    friend void init_platform(void); // from startup.cpp
    friend class Msp432;
private:
    constexpr explicit Wdt() noexcept : reg_addr(WDT_BASE) {}

    inline WdtRegisters& reg() const noexcept
    {
        return *reinterpret_cast<WdtRegisters*>(reg_addr);
    }

    const size_t reg_addr;
};
