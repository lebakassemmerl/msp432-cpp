// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include "helpers.h"
#include "wdt.h"

void Wdt::enable() const noexcept
{
    reg().ctl.modify(
        wdtregs::ctl::hold.value(1) + 
        wdtregs::ctl::pw.value(wdtregs::ctl::KEY_WRITE));
}

void Wdt::disable() const noexcept
{
    reg().ctl.modify(
        wdtregs::ctl::hold.value(1) + 
        wdtregs::ctl::pw.value(wdtregs::ctl::KEY_WRITE));
}

void Wdt::tickle() const noexcept
{
    reg().ctl.modify(
        wdtregs::ctl::cntcl.value(1) + 
        wdtregs::ctl::pw.value(wdtregs::ctl::KEY_WRITE));
}
