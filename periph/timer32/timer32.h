// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstddef>

#include "cs.h"
#include "err.h"
#include "helpers.h"
#include "timer32_regs.h"

class Timer32 {
public:
    Err init(bool use_interrupt, void* cookie, void (*callback)(void *cookie) noexcept) noexcept;
    Err start() noexcept;
    void stop() noexcept;
    Err set_frequency(uint32_t freq_hz, const Cs& cs) noexcept;

    inline uint32_t get_frequency() const noexcept { return freq; }
    inline uint32_t get_load_register() noexcept { return reg().load.get(); }
    inline bool is_running() noexcept { return (status & STATUS_RUNNING) > 0; }
    inline bool is_initialized() noexcept { return (status & STATUS_INITIALIZED) > 0; }

    friend class Msp432;
    friend void periph_int_handler(void) noexcept;
private:
    static constexpr uint8_t STATUS_INITIALIZED = hlp::bit<uint8_t>(0);
    static constexpr uint8_t STATUS_RUNNING = hlp::bit<uint8_t>(1);
    static constexpr uint8_t STATUS_USE_INTERRUPT = hlp::bit<uint8_t>(2);

    constexpr explicit Timer32(size_t reg_base) noexcept
        : status(0), freq(0), cookie(nullptr), cb(nullptr), reg_base(reg_base) {}

    void handle_interrupt() noexcept;

    inline Timer32Registers& reg() noexcept
    {
        return *reinterpret_cast<Timer32Registers*>(reg_base);
    }

    uint8_t status;
    uint32_t freq;
    void* cookie;
    void (*cb)(void* cookie) noexcept;
    const size_t reg_base;
};
