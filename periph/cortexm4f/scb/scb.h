// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>
#include <cstdint>

#include "scb_regs.h"

struct CpuInfo {
    uint8_t impl_code;
    uint8_t variant;
    uint8_t constant;
    uint16_t part_number;
    uint8_t rev_number;
};

class SystemControlBlock {
public:
    SystemControlBlock(const SystemControlBlock&) = delete;
    SystemControlBlock(const SystemControlBlock&&) = delete;
    SystemControlBlock& operator=(const SystemControlBlock&) = delete;
    SystemControlBlock& operator=(const SystemControlBlock&&) = delete;
    constexpr ~SystemControlBlock() noexcept {}

    void set_vector_table_offset(uint32_t offset) noexcept;
    void enable_fpu() noexcept;
    CpuInfo get_cpu_info() const noexcept;

    friend class CortexM4F;
private:
    constexpr explicit SystemControlBlock() noexcept : reg_addr(SCB_BASE) {}

    inline ScbRegisters& reg() const noexcept
    {
        return *reinterpret_cast<ScbRegisters*>(reg_addr);
    }

    const size_t reg_addr;
};
