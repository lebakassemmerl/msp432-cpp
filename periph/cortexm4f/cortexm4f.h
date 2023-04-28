// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include "helpers.h"
#include "register.h"

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

    Systick& systick() noexcept { return m_systick; }
    Nvic& nvic() noexcept { return m_nvic; }
    SystemControlBlock& scb() noexcept { return m_scb; }

    friend class Msp432;
private:
    constexpr explicit CortexM4F() noexcept: m_systick(), m_scb(), m_nvic() {}

    Systick m_systick;
    SystemControlBlock m_scb;
    Nvic m_nvic;
};