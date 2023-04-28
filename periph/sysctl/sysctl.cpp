// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include "helpers.h"
#include "sysctl.h"
#include "sysctl_regs.h"

void SysCtl::enable_all_sram_banks() const noexcept
{
    reg().sram_banken.modify(sysctlregs::sram_banken::bnk7_en.value(1));
}