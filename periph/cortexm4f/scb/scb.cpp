// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <cstddef>
#include <cstdint>

#include "cm4f.h"
#include "scb.h"
#include "scb_regs.h"

void SystemControlBlock::set_vector_table_offset(uint32_t offset) noexcept
{
    reg().vtor.set(offset << 10);
}

CpuInfo SystemControlBlock::get_cpu_info() const noexcept
{
    CpuInfo info{};
    uint32_t raw = reg().cpuid.get();

    info.impl_code = static_cast<uint8_t>((raw >> 24) & 0xFF);
    info.variant = (raw >> 20) & 0x0F;
    info.constant = (raw >> 16) & 0x0F;
    info.part_number = (raw >> 4) & 0xFFF;
    info.rev_number = raw & 0x0F;

    return info;
}
