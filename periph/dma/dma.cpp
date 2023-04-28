// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#include <bit>

#include "err.h"
#include "helpers.h"
#include "register.h"

#include "dma_regs.h"
#include "dma.h"

void Dma::init() noexcept
{
    // enable the DMA module
    reg().cfg.set(dmaregs::cfg::masten.value(1));

    uint32_t addr = reinterpret_cast<uint32_t>(&dma_ctrl[0]);
    reg().ctlbase.set(addr);
}

void Dma::handle_interrupt(int num) noexcept
{
    uint32_t irq = reg().int0_srcflg.get();
    for (size_t i = 0; i <  DMA_CHANNEL_CNT; i++) {
        uint32_t bit = 1 << i;
        if (bit & irq) {
            // clear interrupt flag
            reg().int0_clrflg.set(bit);

            // call interrupt handler
            dma_channels[i].handle_interrupt();
        }
    }
}
