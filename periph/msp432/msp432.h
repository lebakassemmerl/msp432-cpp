// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include "err.h"
#include "helpers.h"
#include "register.h"

#include "cortexm4f.h"
#include "cs.h"
#include "dma.h"
#include "flctl.h"
#include "gpio.h"
#include "pcm.h"
#include "sysctl.h"
#include "timer32.h"
#include "timer32_regs.h"
#include "usci.h"
#include "uscia_regs.h"
#include "uscib_regs.h"
#include "wdt.h"

class Msp432 {
public:
    static constexpr Msp432& instance() noexcept { return chip; }
    Msp432(const Msp432&) = delete;
    Msp432(const Msp432&&) = delete;
    Msp432& operator=(const Msp432&) = delete;
    Msp432& operator=(const Msp432&&) = delete;
    constexpr ~Msp432() noexcept {}

    constexpr CortexM4F& cortexm4f() noexcept { return m_cortexm4f; }
    constexpr Cs& cs() noexcept { return m_cs; }
    constexpr Dma& dma() noexcept { return m_dma; }
    constexpr FlCtl& flctl() noexcept { return m_flctl; }
    constexpr GpioPins& gpio_pins() noexcept { return m_pins; }
    constexpr Pcm& pcm() noexcept { return m_pcm; }
    constexpr SysCtl& sysctl() noexcept { return m_sysctl; }
    constexpr Wdt& wdt() noexcept { return m_wdt; }

    constexpr Usci<UsciARegisters>& uscia0() noexcept { return m_uscia0; }
    constexpr Usci<UsciARegisters>& uscia1() noexcept { return m_uscia1; }
    constexpr Usci<UsciARegisters>& uscia2() noexcept { return m_uscia2; }
    constexpr Usci<UsciARegisters>& uscia3() noexcept { return m_uscia3; }

    constexpr Usci<UsciBRegisters>& uscib0() noexcept { return m_uscib0; }
    constexpr Usci<UsciBRegisters>& uscib1() noexcept { return m_uscib1; }
    constexpr Usci<UsciBRegisters>& uscib2() noexcept { return m_uscib2; }
    constexpr Usci<UsciBRegisters>& uscib3() noexcept { return m_uscib3; }

    constexpr Timer32& t32_1() noexcept { return m_t32_1; }
    constexpr Timer32& t32_2() noexcept { return m_t32_2; }

    void init() noexcept;
    void delay_ms(uint32_t delay) noexcept { m_cortexm4f.systick().delay_ms(delay); }
    void enable_interrupts() noexcept;
    void disable_interrupts() noexcept;
private:
    static constinit Msp432 chip;
    consteval explicit Msp432() noexcept
        : m_cortexm4f(), m_cs(), m_dma(), m_flctl(), m_pins(), m_pcm(), m_sysctl(), m_wdt(), 
        m_uscia0(USCIA0_BASE), m_uscia1(USCIA1_BASE), m_uscia2(USCIA2_BASE), m_uscia3(USCIA3_BASE),
        m_uscib0(USCIB0_BASE), m_uscib1(USCIB1_BASE), m_uscib2(USCIB2_BASE), m_uscib3(USCIB3_BASE),
        m_t32_1(TIMER32_1_BASE), m_t32_2(TIMER32_2_BASE) {}

    void start_systick() noexcept;
    void init_clock() noexcept;

    CortexM4F m_cortexm4f;
    Cs m_cs;
    Dma m_dma;
    FlCtl m_flctl;
    GpioPins m_pins;
    Pcm m_pcm;
    SysCtl m_sysctl;
    Wdt m_wdt;
    Usci<UsciARegisters> m_uscia0;
    Usci<UsciARegisters> m_uscia1;
    Usci<UsciARegisters> m_uscia2;
    Usci<UsciARegisters> m_uscia3;
    Usci<UsciBRegisters> m_uscib0;
    Usci<UsciBRegisters> m_uscib1;
    Usci<UsciBRegisters> m_uscib2;
    Usci<UsciBRegisters> m_uscib3;
    Timer32 m_t32_1;
    Timer32 m_t32_2;
};
