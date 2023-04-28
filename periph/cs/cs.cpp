// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include "cs.h"
#include "cs_regs.h"
#include "helpers.h"
#include "register.h"


void Cs::before_peripheral_access() const noexcept
{
    // write key to key register to unlock register access
    reg().key.set(csregs::key::key.value(csregs::key::KEY));
}

void Cs::after_peripheral_access() const noexcept
{
    // lock register access by writing anything but the key to the key register
    reg().key.set(csregs::key::key.value(0));
}

void Cs::set_mclk_48mhz() noexcept
{
    PeripheralManager pm{*this};
    auto& regs = pm.periph().reg();
    
    // set HFXT to 40-48MHz range
    regs.ctl2.modify(csregs::ctl2::hfxtfreq.value(6));

    // set HFXT (48MHz) as MCLK source
    regs.ctl1.modify(csregs::ctl1::selm.value(5) + csregs::ctl1::divm.value(0));

    // wait until the clock is set and the fault interrupts stop occurring
    while ((regs.ifg.get() & csregs::ifg::hfxt.raw_value(1)) > 0) {
        regs.clrifg.set(csregs::ifg::hfxt.value(1) + csregs::ifg::fcnthf.value(1));
    }

    mclk = 48000000;
}

// Setup the subsystem master clock (HSMCLK) to 1/4 of the master-clock -> 12MHz
void Cs::set_hsmclk_12mhz() noexcept
{
    PeripheralManager<Cs> pm{*this};
    auto& regs = pm.periph().reg();

    // use HFXT (48MHz) as source for HSMCLK and divide it by 4 -> 12MHz
    regs.ctl1.modify(csregs::ctl1::sels.value(5) + csregs::ctl1::divhs.value(2));

    hsmclk = 12000000;
}

// Setup the low-speed subsystem master clock (SMCLK) to 1/4 of the master-clock -> 12MHz
void Cs::set_smclk_12mhz() noexcept
{
    PeripheralManager pm{*this};
    auto& regs = pm.periph().reg();

    // use HFXT (48MHz) as source for SMCLK and divide it by 4 -> 12MHz
    regs.ctl1.modify(csregs::ctl1::sels.value(5) + csregs::ctl1::divs.value(2));

    smclk = 12000000;
}

// Setup the auxiliary clock (ACLK) to 32.768kHz
void Cs::set_aclk_32khz() noexcept
{
    PeripheralManager pm{*this};
    auto& regs = pm.periph().reg();

    // use HFXT (48MHz) as source for SMCLK and divide it by 4 -> 12MHz
    regs.ctl1.modify(csregs::ctl1::sela.value(0) + csregs::ctl1::diva.value(0));

    aclk = 32768;
}
