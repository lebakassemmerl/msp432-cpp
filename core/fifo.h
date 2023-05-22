// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 * 
 * This file implements a single producer / single consumer FIFO in form of a ringbuffer. Intended
 * usage is for example a queue within a bus-driver (e.g. SPI). It is designed with a lockfree
 * approach but I am ABSOLUTELY NOT SURE if there aren't any bugs inside (probably there are).
 * However was tested with the test test/test_fifo.cpp. If it is used in a different way than
 * intended, it probably will result in undefined behavior!
 * 
 * Producer-functions:
 * - push()
 * - emplace()
 * 
 * Consumer functions:
 * - peek()
 * - peek_ref()
 * - pop_elem()
 * - pop()
 * 
 * Functions for both:
 * - used()
 * - free()
 * - is_full()
 * - is_empty()
 */

#pragma once

#include <array>
#include <atomic>
#include <expected>
#include <functional>
#include <cstddef>
#include <optional>
#include <utility>

#include "err.h"
#include "helpers.h"

template<typename T, size_t N>
class Fifo {
public:
    static_assert(hlp::is_powerof2<N>(), "N must be power of 2");

    constexpr explicit Fifo() noexcept : buf(), head(0), tail(0) {}
    constexpr ~Fifo() noexcept {}

    Err push(const T& val) noexcept
    {
        if (is_full())
            return Err::NoMem;

        // We have to copy in the value before incrementing the head-pointer since otherwise it can
        // happen, that the consumer is reading out the data before we copied it into the FIFO.
        buf[head.load()] = val;
        fetch_add(head, 1);

        return Err::Ok;
    }

    template<typename... Args>
    Err emplace(Args&&... args) noexcept
    {
        if (is_full())
            return Err::NoMem;

        // same explaination as in push()
        new(&buf[head.load()]) T{std::forward<Args>(args)...};
        fetch_add(head, 1);

        return Err::Ok;
    }

    std::expected<T, Err> peek() noexcept
    {
        if (is_empty())
            return std::unexpected{Err::Empty};

        // This function can only work properly (without ending in undefined behavior) if this FIFO
        // is used as a single producer / single consumer data-structure. Otherwise the referenced
        // object from the FIFO cannot be valid since the tail-pointer could change and the
        // head-pointer too.
        return std::expected<T, Err>{buf[tail.load()]};
    }

    std::expected<std::reference_wrapper<T>, Err> peek_ref() noexcept
    {
        if (is_empty())
            return std::unexpected{Err::Empty};

        // same explaination as in peek()
        return std::expected<std::reference_wrapper<T>, Err>{
            std::reference_wrapper<T>{buf[tail.load()]}};
    }

    std::expected<T, Err> pop_elem() noexcept
    {
        size_t old;
        size_t ny;
        T ret;

        if (is_empty())
            return std::unexpected{Err::Empty};

        old = tail.load(std::memory_order::seq_cst);
        do {
            // Since we are only copying data out of the buffer, this should be okay and not destroy
            // anything within the FIFO. We try to copy out the data until we get a valid
            // tail-pointer.
            ret = buf[old];
            ny = (old + 1) & (N - 1);
        } while (!tail.compare_exchange_weak(old, ny, std::memory_order::seq_cst));

        return std::expected<T, Err>{ret};
    }

    Err pop() noexcept
    {
        if (is_empty())
            return Err::Empty;

        // we only need to update the tail-pointer for popping out an element
        fetch_add(tail, 1);
        return Err::Ok;
    }

    inline bool is_full() const noexcept { return used() == (N - 1); }
    inline bool is_empty() const noexcept { return used() == 0; }
    inline size_t free() const noexcept { return (N - 1 - used()); }
    inline size_t used() const noexcept
    {
        // To get the 'worst-case-scenario', where it is more likely that the queue is full, we have
        // to read tail first.
        size_t t = tail.load(std::memory_order::seq_cst);
        size_t h = head.load(std::memory_order::seq_cst);

        if (h >= t)
            return h - t;
        else
            return N + h - t;
    }

private:
    inline void fetch_add(std::atomic<size_t>& a, size_t val) noexcept
    {
        size_t ny;
        size_t old = a.load(std::memory_order::seq_cst);

        do {
            ny = (old + val) & (N - 1);
        } while (!a.compare_exchange_weak(old, ny, std::memory_order::seq_cst));
    }

    std::array<T, N> buf;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};
