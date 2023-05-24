// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include "helpers.h"
#include "register.h"

#include "fpu/fpu.h"
#include "nvic/nvic.h"
#include "scb/scb.h"
#include "systick/systick.h"

class CortexM4F {
public:
    CortexM4F(const CortexM4F&) = delete;
    CortexM4F(const CortexM4F&&) = delete;
    CortexM4F& operator=(const CortexM4F&) = delete;
    CortexM4F& operator=(const CortexM4F&&) = delete;
    constexpr ~CortexM4F() noexcept {}

    Fpu& fpu() noexcept { return m_fpu; }
    Nvic& nvic() noexcept { return m_nvic; }
    SystemControlBlock& scb() noexcept { return m_scb; }
    Systick& systick() noexcept { return m_systick; }

    friend class Msp432;
private:
    constexpr explicit CortexM4F() noexcept: m_fpu(), m_nvic(), m_scb(), m_systick() {}

    Fpu m_fpu;
    Nvic m_nvic;
    SystemControlBlock m_scb;
    Systick m_systick;
};