// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "pcm_regs.h"

class Pcm {
public:
    Pcm(const Pcm&) = delete;
    Pcm(const Pcm&&) = delete;
    Pcm& operator=(const Pcm&) = delete;
    Pcm& operator=(const Pcm&&) = delete;
    constexpr ~Pcm() {};

    void set_high_power() const noexcept;

    friend void init_platform(void); // from startup.cpp
    friend class Msp432;
private:
    constexpr explicit Pcm() noexcept : reg_addr(PCM_BASE) {}

    inline PcmRegisters& reg() const noexcept
    {
        return *reinterpret_cast<PcmRegisters*>(reg_addr);
    }

    const size_t reg_addr;
};