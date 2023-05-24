// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "helpers.h"
#include "register.h"

#include "nvic_regs.h"

class Nvic {
public:
    Nvic(const Nvic&) = delete;
    Nvic(const Nvic&&) = delete;
    Nvic& operator=(const Nvic&) = delete;
    Nvic& operator=(const Nvic&&) = delete;
    constexpr ~Nvic() noexcept {}

    uint64_t get_pending_interrupts() noexcept;
    void enable_all_interrupts() noexcept;
    void disable_all_interrupts() noexcept;
    void clear_all_pending() noexcept;
    void clear_pending(size_t idx) noexcept;

    friend class CortexM4F;
private:
    constexpr explicit Nvic() noexcept : reg_addr(NVIC_BASE) {}

    inline NvicRegisters& reg() const noexcept
    {
        return *reinterpret_cast<NvicRegisters*>(reg_addr);
    }

    const size_t reg_addr;
};