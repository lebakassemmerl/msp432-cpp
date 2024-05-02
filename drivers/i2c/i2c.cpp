// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#include <cstdint>
#include <span>

#include "err.h"
#include "i2c.h"
#include "usci.h"
#include "uscib_regs.h"

Err I2cMaster::init() noexcept
{
    if (initialized)
        return Err::AlreadyInitialized;

    // TODO: clock prescaler calculation
    usci.reg().brw.set(0);

    // enable automatic STOP condition generation
    usci.reg().ctlw1.modify(uscibregs::ctlw1::astp.value(0));

    usci.reg().ctlw0.modify(
        uscibregs::ctlw0::mm.value(0) +     // single master environment
        uscibregs::ctlw0::mst.value(1) +    // Master mode
        uscibregs::ctlw0::mode.value(3) +   // I2C mode
        uscibregs::ctlw0::ssel.value(2) +   // use SMCLK as I2C clock-source
        uscibregs::ctlw0::swrst.value(0)    // disable software-reset -> enable module
    );
}
