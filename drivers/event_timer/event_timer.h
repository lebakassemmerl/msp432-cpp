/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 * 
 * This EventTimer configures a hardware-timer to fire an interrupt every 1ms. The API is designed
 * to register multiple Events which will fire after a configured amount of milliseconds (interval).
 * Be aware, that the callbacks of this EventTimer are running in interrupt-context!
 * 
 * The API is only safe to use if the events are registered outside of an interrupt-context,
 * otherwise this could end up in undefined behavior.
 */

#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <expected>
#include <utility>

#include "cs.h"
#include "timer32.h"

class EventTimer {
public:
    class Event {
    public:
        // allow creating an invalid instance of an event
        constexpr explicit Event() noexcept : ev(0xFF) {}

        // moving the event is okay since the event is still valid then
        constexpr Event(const Event&& other) noexcept : ev(std::move(other.ev)) {}

        // If we call a move constructor on a non-constant reference, we additionally set the
        // event-ID to 0xFF which is an invalid value.
        constexpr Event(Event&& other) noexcept : ev(std::exchange(other.ev, 0xFF)) {}

        // move-assignement is also allowed
        constexpr Event& operator=(const Event&& other) noexcept
        {
            ev = std::move(other.ev);
            return *this;
        }

        // we don't want an event to be copied or modifed
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;

        friend class EventTimer;
    private:
        constexpr explicit Event(uint8_t idx) noexcept : ev(idx) {}

        uint8_t ev;
    };

    constexpr explicit EventTimer(Timer32& t32) noexcept
        : initialized(false), ev_list(), t32(t32) {}

    void init(const Cs& cs) noexcept;
    Err start_event(const Event& ev) noexcept;
    Err stop_event(const Event& ev) noexcept;
    std::expected<EventTimer::Event, Err> register_event(
        uint16_t interval_ms, void* cookie, void (*elapsed_cb)(void* cookie) noexcept) noexcept;
private:
    struct EventEntry {
        constexpr explicit EventEntry() noexcept
            : interval(0), cnt(0), cookie(nullptr), elapsed(nullptr) {}

        uint16_t interval;
        uint16_t cnt;
        void* cookie;
        void (*elapsed)(void* cookie) noexcept;
    };

    template<typename T> requires std::unsigned_integral<T>
    struct EventList {
        constexpr explicit EventList() noexcept : enabled(0), used(0), events() {}

        std::atomic<T> enabled;
        std::atomic<uint8_t> used;
        std::array<EventEntry, sizeof(T) * 8> events;
    };

    static void timer_cb(void* cookie) noexcept;

    bool initialized;
    EventList<uint32_t> ev_list;
    Timer32& t32;
};
