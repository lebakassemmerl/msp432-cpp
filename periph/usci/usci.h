// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>
#include <concepts>

#include "err.h"
#include "register.h"
#include "uscia_regs.h"
#include "uscib_regs.h"

template<typename T>
concept UsciReg = std::same_as<UsciARegisters, T> || std::same_as<UsciBRegisters, T>;

template<typename T> requires UsciReg<T>
class Usci {
public:
    Usci(const Usci<T>&) = delete;
    Usci(const Usci<T>&&) = delete;
    Usci& operator=(const Usci<T>&) = delete;
    Usci& operator=(const Usci<T>&&) = delete;
    constexpr ~Usci() noexcept {}

    constexpr inline T& reg() noexcept
    {
        return *reinterpret_cast<T*>(reg_base);
    }

    err::Err register_irq_handler(void (*fn)(void*) noexcept, void *inst) noexcept
    {
        if ((!inst) || (!fn))
            return err::Err::NullPtr;

        irq_handler = fn;
        instance = inst;

        return err::Err::Ok;
    }

    // Needed for the SPI implementation
    constexpr ReadWrite<uint16_t>& ctlw0() noexcept { return reg().ctlw0; }
    constexpr ReadWrite<uint16_t>& brw() noexcept { return reg().brw; }
    constexpr ReadWrite<uint16_t>& statw() noexcept { return reg().statw; }
    constexpr ReadOnly<uint16_t>& rxbuf() noexcept { return reg().rxbuf; }
    constexpr ReadWrite<uint16_t>& txbuf() noexcept { return reg().txbuf; }
    constexpr ReadWrite<uint16_t>& ie() noexcept { return reg().ie; }
    constexpr ReadWrite<uint16_t>& ifg() noexcept { return reg().ifg; }

    friend class Msp432;
    friend void periph_int_handler(void) noexcept;
private:
    constexpr explicit Usci(size_t base) noexcept
        : reg_base(base), instance(nullptr), irq_handler(nullptr) {}

    void handle_interrupt(void) noexcept
    {
        if (irq_handler && instance)
            irq_handler(instance);
    }

    const size_t reg_base;
    void *instance;
    void (*irq_handler)(void*) noexcept;
};