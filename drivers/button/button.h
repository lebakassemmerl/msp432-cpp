// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>

#include "err.h"
#include "event_timer.h"
#include "helpers.h"
#include "pin.h"

enum class ButtonEvent : uint8_t {
    None = 0,
    Pressed = hlp::bit<uint8_t>(0),
    Released = hlp::bit<uint8_t>(1),
    LongPress = hlp::bit<uint8_t>(2),
};

class Button {
public:
    constexpr explicit Button(Pin& pin, EventTimer& et, bool active_low) noexcept
        : shift_reg(0), long_press_cnt(0), prev_event(ButtonEvent::None),
        status(static_cast<uint8_t>(active_low) << ACTIVE_LOW_BIT), btn_cb(nullptr),
        cookie(nullptr), timer_event(), et(et), pin(pin) {}

    Err init(void* cookie, void (*btn_event)(ButtonEvent ev, void* cookie) noexcept) noexcept;
    Err enable_event(ButtonEvent ev) noexcept;
    Err disable_event(ButtonEvent ev) noexcept;

private:
    static constexpr size_t ACTIVE_LOW_BIT = 6;
    static constexpr uint8_t INITIALIZED = hlp::bit<uint8_t>(7);
    static constexpr uint8_t ACTIVE_LOW = hlp::bit<uint8_t>(ACTIVE_LOW_BIT);
    static constexpr uint8_t EVENT_MASK = hlp::mask<uint8_t>(2, 0);

    static void timer_cb(void* cookie) noexcept;
    void handle_timer_event() noexcept;

    inline bool is_initialized() const noexcept { return (status & INITIALIZED) > 0; }
    inline bool is_active_low() const noexcept { return (status & ACTIVE_LOW) > 0; }
    inline bool event_active() const noexcept { return (status & EVENT_MASK) > 0; }

    uint8_t shift_reg;
    uint8_t long_press_cnt;
    ButtonEvent prev_event;
    volatile uint8_t status; // store the button-events, initialized and active-low in here

    void (*btn_cb)(ButtonEvent ev, void* cookie) noexcept;
    void* cookie;

    EventTimer::Event timer_event;
    EventTimer& et;
    Pin& pin;
};
