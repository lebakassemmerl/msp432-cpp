// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <expected>

#include "button.h"
#include "err.h"
#include "event_timer.h"
#include "helpers.h"
#include "pin.h"

constexpr uint16_t SAMPLE_TIME = 10; // 10ms
constexpr size_t SAMPLES = 5; // results in 50ms debounce time
constexpr uint8_t LONG_BUTTON_PRESS = 150; // 10ms * 150 = 1.5s

Err Button::init(void* cookie, void (*btn_cb)(ButtonEvent ev, void* cookie) noexcept) noexcept
{
    std::expected<EventTimer::Event, Err> ev;

    if (!btn_cb)
        return Err::NullPtr;

    ev = et.register_event(10, this, Button::timer_cb);
    if (!ev.has_value())
        return ev.error();

    pin.make_input();
    if (is_active_low())
        pin.set_pull_mode(PullMode::PullUp);
    else
        pin.set_pull_mode(PullMode::PullDown);

    timer_event = std::move(ev.value());
    this->btn_cb = btn_cb;
    this->cookie = cookie;

    status |= INITIALIZED;
    return Err::Ok;
}

Err Button::enable_event(ButtonEvent ev) noexcept
{
    if (!is_initialized())
        return Err::NotInitialized;

    // if no event is active, we have to start the timer
    if (!event_active())
        et.start_event(timer_event);

    status |= static_cast<uint8_t>(ev);
    return Err::Ok;
}

Err Button::disable_event(ButtonEvent ev) noexcept
{
    if (!is_initialized())
        return Err::NotInitialized;

    status &= ~static_cast<uint8_t>(ev);
    if (!event_active()) {
        shift_reg = 0;
        long_press_cnt = 0;
        et.stop_event(timer_event);
    }

    return Err::Ok;
}

void Button::timer_cb(void* cookie) noexcept
{
    Button* btn = reinterpret_cast<Button*>(cookie);
    btn->handle_timer_event();
}

void Button::handle_timer_event() noexcept
{
    constexpr uint8_t SHREG_MASK = hlp::mask<uint8_t>(SAMPLES - 1, 0);

    bool new_event = false;
    bool high;
    bool low;

    shift_reg <<= 1;
    shift_reg |= static_cast<uint8_t>(pin.read());

    high = (shift_reg & SHREG_MASK) == SHREG_MASK;
    low = (shift_reg & SHREG_MASK) == 0;

    if (!high && !low) {
        prev_event = ButtonEvent::None;
        long_press_cnt = 0;
        return;
    }

    if ((low && is_active_low()) || (high && !is_active_low())) {
        // button is pressed
        if (prev_event == ButtonEvent::Pressed) {
            long_press_cnt++;
            if (long_press_cnt > LONG_BUTTON_PRESS) {
                long_press_cnt = 0;
                prev_event = ButtonEvent::LongPress;
                new_event = true;
            }
        } else if ((prev_event == ButtonEvent::Released) || (prev_event == ButtonEvent::None)) {
            prev_event = ButtonEvent::Pressed;
            new_event = true;
        }
    } else if ((low && !is_active_low()) || (high && is_active_low())) {
        // button is not pressed

        long_press_cnt = 0;
        if (prev_event == ButtonEvent::Released)
            return;

        prev_event = ButtonEvent::Released;
        new_event = true;
    }

    if (new_event && (status & static_cast<uint8_t>(prev_event)))
        btn_cb(prev_event, cookie);
}
