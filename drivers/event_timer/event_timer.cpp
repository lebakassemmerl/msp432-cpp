// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <expected>

#include "err.h"
#include "event_timer.h"
#include "timer32.h"

void EventTimer::init(const Cs& cs) noexcept
{
    t32.init();
    t32.set_frequency(1000, cs);
    t32.set_callback(EventTimer::timer_cb, this);

    initialized = true;
}

std::expected<EventTimer::Event, Err> EventTimer::register_event(
    uint16_t interval_ms, void* cookie, void (*elapsed_cb)(void* cookie) noexcept) noexcept
{
    uint8_t idx;

    if (interval_ms == 0)
        return std::unexpected{Err::OutOfRange};

    if (!elapsed_cb)
        return std::unexpected{Err::NullPtr};

    idx = ev_list.used.fetch_add(1, std::memory_order::seq_cst);
    if (idx >= ev_list.events.size()) {
        // We are out of range and incremented the index before. If we do this too oftent, the index
        // wraps around and becomes valid again. Thus, we reset the index to a safe value which is
        // bigger than the size of the event-array.
        ev_list.used.store(static_cast<uint8_t>(ev_list.events.size() + 1));
        return std::unexpected{Err::NoMem};
    }

    ev_list.events[idx].elapsed = elapsed_cb;
    ev_list.events[idx].cookie = cookie;
    ev_list.events[idx].interval = interval_ms;
    ev_list.events[idx].cnt = 0;

    return std::expected<EventTimer::Event, Err>{EventTimer::Event{idx}};
}

Err EventTimer::start_event(const Event& ev) noexcept
{
    if (!initialized)
        return Err::NotInitialized;

    auto old = ev_list.enabled.load();

    using T = decltype(old);

    T ny;
    do {
        ny = old | (static_cast<T>(1U) << static_cast<T>(ev.ev));
    } while (!ev_list.enabled.compare_exchange_weak(old, ny, std::memory_order::seq_cst));

    if (!t32.is_running())
        t32.start();

    return Err::Ok;
}

Err EventTimer::stop_event(const Event& ev) noexcept
{
    if (!initialized)
        return Err::NotInitialized;

    auto old = ev_list.enabled.load();

    using T = decltype(old);

    T ny;
    do {
        ny = old & ~(static_cast<T>(1U) << static_cast<T>(ev.ev));
    } while (!ev_list.enabled.compare_exchange_weak(old, ny, std::memory_order::seq_cst));

    if (ny == 0)
        t32.stop();

    return Err::Ok;
}

void EventTimer::timer_cb(void *cookie) noexcept
{
    EventTimer *et = reinterpret_cast<EventTimer*>(cookie);

    auto enabled = et->ev_list.enabled.load();
    for (uint8_t i = 0; i < et->ev_list.used.load(); i++) {
        using T = decltype(enabled);

        EventEntry& ev = et->ev_list.events[i];

        if ((enabled & (static_cast<T>(1UL) << static_cast<T>(i))) == 0)
            continue;

        ev.cnt++;
        if (ev.cnt >= ev.interval) {
            ev.cnt = 0;
            ev.elapsed(ev.cookie);
        }
    }
}
