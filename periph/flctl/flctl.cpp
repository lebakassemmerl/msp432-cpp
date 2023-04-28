// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include "helpers.h"
#include "flctl.h"
#include "flctl_regs.h"

void FlCtl::set_waitstates(WaitStates ws) const noexcept
{
    reg().bank0_rdctl.modify(flctlregs::bank0_rdctl::wait.value(static_cast<uint32_t>(ws)));
    reg().bank1_rdctl.modify(flctlregs::bank1_rdctl::wait.value(static_cast<uint32_t>(ws)));
}

void FlCtl::set_buffering(bool enable) const noexcept
{
    reg().bank0_rdctl.modify(flctlregs::bank0_rdctl::bufd.value(static_cast<uint32_t>(enable)));
    reg().bank0_rdctl.modify(flctlregs::bank0_rdctl::bufi.value(static_cast<uint32_t>(enable)));

    reg().bank1_rdctl.modify(flctlregs::bank1_rdctl::bufd.value(static_cast<uint32_t>(enable)));
    reg().bank1_rdctl.modify(flctlregs::bank1_rdctl::bufi.value(static_cast<uint32_t>(enable)));
} 
