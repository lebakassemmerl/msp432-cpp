// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "sysctl_regs.h"

class SysCtl {
public:
    SysCtl(const SysCtl&) = delete;
    SysCtl(const SysCtl&&) = delete;
    SysCtl& operator=(const SysCtl&) = delete;
    SysCtl& operator=(const SysCtl&&) = delete;
    constexpr ~SysCtl() {};

    void enable_all_sram_banks() const noexcept;

    friend void init_platform(void); // from startup.cpp
    friend class Msp432;
private:
    constexpr explicit SysCtl() noexcept : reg_addr(SYSCTL_BASE) {}
    inline SysCtlRegisters& reg() const noexcept
    {
        return *reinterpret_cast<SysCtlRegisters*>(reg_addr);
    }

    const size_t reg_addr;
};