// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#include "err.h"
#include "helpers.h"
#include "register.h"

#include "msp432.h"
#include "cs.h"
#include "flctl.h"
#include "gpio.h"
#include "pcm.h"
#include "sysctl.h"
#include "wdt.h"

Msp432 Msp432::chip{};

void Msp432::init() noexcept
{
    init_clock();

    m_cortexm4f.systick().start(m_cs.m_clk());
    m_cortexm4f.fpu().enable();
    m_cortexm4f.fpu().set_rounding_mode(Fpu::RoundingMode::Nearest);

    m_dma.init();

    enable_interrupts();
}

void Msp432::init_clock() noexcept
{
    // setup the gpio pins in order to use the HFXT oscillator (48MHz)
    gpio_pins().pin(PinNr::PJ_2).enable_primary_function();
    gpio_pins().pin(PinNr::PJ_3).enable_primary_function();

    // setup the gpio pins in order to use the LFXT oscillator (32.768MHz)
    gpio_pins().pin(PinNr::PJ_0).enable_primary_function();
    gpio_pins().pin(PinNr::PJ_1).enable_primary_function();

    cs().set_mclk_48mhz();
    cs().set_hsmclk_12mhz();
    cs().set_smclk_12mhz();
    cs().set_aclk_32khz();
}


void Msp432::enable_interrupts() noexcept
{
    m_cortexm4f.nvic().disable_all_interrupts();
    m_cortexm4f.nvic().clear_all_pending();
    m_cortexm4f.nvic().enable_all_interrupts();
    __asm__("CPSIE I"); // enable global interrupts
}

void Msp432::disable_interrupts() noexcept
{
    m_cortexm4f.nvic().disable_all_interrupts();
    __asm__("CPSID I"); // disable global interrupts
}

void Msp432::delay_ms(uint32_t delay) noexcept
{
    uint64_t until = m_cortexm4f.systick().uptime_ms() + static_cast<uint64_t>(delay);
    while (until > m_cortexm4f.systick().uptime_ms())
        __asm__("nop");
}