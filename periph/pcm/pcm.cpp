// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include "helpers.h"
#include "pcm.h"
#include "pcm_regs.h"

void Pcm::set_high_power() const noexcept
{
    // In order to enable a 48MHz clock for the CPU it's necessary to configure the DCDC_VCORE1
    // mode. A direct transition from LDO_VCORE0 to DCDC_VCORE1 is invalid, thus switch to
    // LDO_VCORE1 first and then switch to DCDC_VCORE1.

    while ((reg().ctl1.get() & pcmregs::ctl1::pmr_busy.raw_value(1)) > 0);

    // change to LDO_VCORE1
    reg().ctl0.modify(pcmregs::ctl0::key.value(pcmregs::KEY) + pcmregs::ctl0::amr.value(1));

    while ((reg().ctl1.get() & pcmregs::ctl1::pmr_busy.raw_value(1)) > 0);

    // change to DCDC_VCORE1
    reg().ctl0.modify(pcmregs::ctl0::key.value(pcmregs::KEY) + pcmregs::ctl0::amr.value(1));

    while ((reg().ctl1.get() & pcmregs::ctl1::pmr_busy.raw_value(5)) > 0);
}
