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

class Usci {
public:
    Usci(const Usci&) = delete;
    Usci(const Usci&&) = delete;
    Usci& operator=(const Usci&) = delete;
    Usci& operator=(const Usci&&) = delete;
    constexpr ~Usci() noexcept {}

    // Needed for the SPI implementation
    virtual ReadWrite<uint16_t>& ctlw0() = 0;
    virtual ReadWrite<uint16_t>& brw() = 0;
    virtual ReadWrite<uint16_t>& statw() = 0;
    virtual ReadOnly<uint16_t>& rxbuf() = 0;
    virtual ReadWrite<uint16_t>& txbuf() = 0;
    virtual ReadWrite<uint16_t>& ie() = 0;
    virtual ReadWrite<uint16_t>& ifg() = 0;

    Err register_irq_handler(void (*fn)(void*) noexcept, void* handle) noexcept
    {
        if (!fn)
            return Err::NullPtr;

        irq_handler = fn;
        cookie = handle;

        return Err::Ok;
    }
protected:
    constexpr explicit Usci(size_t base) noexcept
        : reg_base(base), cookie(nullptr), irq_handler(nullptr) {}
    
    void handle_interrupt(void) noexcept
    {
        if (irq_handler)
            irq_handler(cookie);
    }

    const size_t reg_base;
    void* cookie;
    void (*irq_handler)(void*) noexcept;
};

class UsciA : public Usci {
public:
    inline UsciARegisters& reg() noexcept
    {
        return *reinterpret_cast<UsciARegisters*>(reg_base);
    }

    constexpr ReadWrite<uint16_t>& ctlw0() noexcept override { return reg().ctlw0; }
    constexpr ReadWrite<uint16_t>& brw() noexcept override { return reg().brw; }
    constexpr ReadWrite<uint16_t>& statw() noexcept override { return reg().statw; }
    constexpr ReadOnly<uint16_t>& rxbuf() noexcept override { return reg().rxbuf; }
    constexpr ReadWrite<uint16_t>& txbuf() noexcept override { return reg().txbuf; }
    constexpr ReadWrite<uint16_t>& ie() noexcept override { return reg().ie; }
    constexpr ReadWrite<uint16_t>& ifg() noexcept override { return reg().ifg; }

    friend class Msp432;
    friend void periph_int_handler(void) noexcept;

private:
    constexpr explicit UsciA(const size_t base) : Usci(base) {}
};

class UsciB : public Usci {
public:
    inline UsciBRegisters& reg() noexcept
    {
        return *reinterpret_cast<UsciBRegisters*>(reg_base);
    }

    constexpr ReadWrite<uint16_t>& ctlw0() noexcept override { return reg().ctlw0; }
    constexpr ReadWrite<uint16_t>& brw() noexcept override { return reg().brw; }
    constexpr ReadWrite<uint16_t>& statw() noexcept override { return reg().statw; }
    constexpr ReadOnly<uint16_t>& rxbuf() noexcept override { return reg().rxbuf; }
    constexpr ReadWrite<uint16_t>& txbuf() noexcept override { return reg().txbuf; }
    constexpr ReadWrite<uint16_t>& ie() noexcept override { return reg().ie; }
    constexpr ReadWrite<uint16_t>& ifg() noexcept override { return reg().ifg; }

    friend class Msp432;
    friend void periph_int_handler(void) noexcept;

private:
    constexpr explicit UsciB(const size_t base) : Usci(base) {}
};
