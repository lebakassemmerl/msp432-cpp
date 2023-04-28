// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "cs_regs.h"


class Cs {
public:
    Cs(const Cs&) = delete;
    Cs(const Cs&&) = delete;
    Cs& operator=(const Cs&) = delete;
    Cs& operator=(const Cs&&) = delete;
    constexpr ~Cs() noexcept {};

    void set_mclk_48mhz() noexcept;
    void set_hsmclk_12mhz() noexcept;
    void set_smclk_12mhz() noexcept;
    void set_aclk_32khz() noexcept;

    uint32_t m_clk() const noexcept { return mclk; }
    uint32_t hsm_clk() const noexcept { return hsmclk; }
    uint32_t sm_clk() const noexcept { return smclk; }
    uint32_t a_clk() const noexcept { return aclk; }

    void before_peripheral_access() const noexcept;
    void after_peripheral_access() const noexcept;

    friend void init_platform(void); // from startup.cpp
    friend class Msp432;
private:
    constexpr explicit Cs() noexcept : reg_addr(CS_BASE), mclk(0), hsmclk(0), smclk(0), aclk(0) {}

    inline CsRegisters& reg() const noexcept
    {
        return *reinterpret_cast<CsRegisters*>(reg_addr);
    }

    const size_t reg_addr;
    uint32_t mclk;
    uint32_t hsmclk;
    uint32_t smclk;
    uint32_t aclk;
};
