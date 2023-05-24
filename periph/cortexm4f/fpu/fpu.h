// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "fpu_regs.h"

class Fpu {
public:
    enum class RoundingMode : uint32_t {
        Nearest = 0,
        PlusInfinity = 1,
        MinusInfinity = 2,
        Zero = 3,
    };

    Fpu(const Fpu&) = delete;
    Fpu(const Fpu&&) = delete;
    Fpu& operator=(const Fpu&) = delete;
    Fpu& operator=(const Fpu&&) = delete;
    constexpr ~Fpu() noexcept {}

    void enable() noexcept;
    void disable() noexcept;
    void set_rounding_mode(RoundingMode mode) noexcept;

    friend class CortexM4F;
    friend void fpu_handler(void) noexcept;
private:
    constexpr explicit Fpu() noexcept : reg_addr(FPU_BASE) {}

    void handle_interrupt() noexcept;
    inline FpuRegisters& reg() const noexcept
    {
        return *reinterpret_cast<FpuRegisters*>(reg_addr);
    }

    const size_t reg_addr;
};
