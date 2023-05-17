// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 * 
 * This file implements a generic FIFO which is compatible for DMA transfers. It is supposed to be
 * used as single producer / single consumer FIFO. All other use-cases will probably result in
 * undefined behavior. The implementation is a lockfree one. The implementation seems to work but I
 * am ABSOLUTELY NOT SURE if it is completely bug-free (probably not).
 * 
 * The producer is supposed to call only put_range() while the consumer has to call peek_range() and
 * drop_range(). The idea to put in data in a ring-buffer and to return a range of continguous
 * data which can be processed by a DMA-channel. It is a similar, but way more simple and probably
 * not that performant approach than bip-buffers
 * (https://www.codeproject.com/articles/3479/the-bip-buffer-the-circular-buffer-with-a-twist).
 * 
 * The current use-case for this type of ring-buffer is the sending part of the UART-driver.
 */

#pragma once

#include <array>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <span>

#include "err.h"
#include "helpers.h"
#include "libc.h"

template<typename T, size_t N> requires std::integral<T>
class DmaFifo {
public:
    static_assert(hlp::is_powerof2<N>(), "N must be power of 2");

    constexpr explicit DmaFifo() noexcept : buf(), range_end(0), head(0), tail(0) {}
    constexpr ~DmaFifo() {}

    // must only be called from the producer!
    Err put_range(std::span<T> data) noexcept
    {
        size_t h;
        if (free() < data.size())
            return Err::NoMem;

        h = fetch_add(head, data.size());

        if ((h + data.size()) > N) {
            size_t tmp = N - h;
            libc::memcpy(&buf[h], &data[0], tmp);
            libc::memcpy(&buf[0], &data[tmp], (data.size() * sizeof(data[0])) - tmp);
        } else {
            libc::memcpy(&buf[h], data.data(), data.size() * sizeof(data[0]));
        }

        return Err::Ok;
    }

    // must only be called from the consumer!
    std::span<T> peek_range() noexcept
    {
        size_t h;
        size_t t;

        if (is_empty())
            return std::span<T>{};

        // load head first, since we are the consumer
        h = head.load(std::memory_order::seq_cst);
        t = tail.load(std::memory_order::seq_cst);

        if (h > t) {
            range_end = h; // the currently returned range ends where head is right now
            return std::span<T>{&buf[t], h - t};
        } else {
            // The contiguous memory ends at the end of the buffer -> return everything from tail to
            // the end of the buffer.
            range_end = 0;
            return std::span<T>{&buf[t], N - t};
        }
    }

    // must only be called from the consumer!
    void drop_range() noexcept { tail.store(range_end, std::memory_order::seq_cst); }

    inline size_t free() const noexcept { return (N - 1 - used()); }
    inline bool is_full() const noexcept { return used() ==  (N - 1); }
    inline bool is_empty() const noexcept { return used() == 0; }
    inline size_t used() const noexcept
    {
        // read tail first, since this could be touched by an interrupt
        size_t t = tail.load(std::memory_order::seq_cst);
        size_t h = head.load(std::memory_order::seq_cst);

        if (h >= t)
            return h - t;
        else
            return N + h - t;
    }

private:
    inline size_t fetch_add(std::atomic<size_t>& a, size_t val) noexcept
    {
        size_t ny;
        size_t old = a.load(std::memory_order::seq_cst);

        do {
            ny = (old + val) & (N - 1);
        } while (!a.compare_exchange_weak(old, ny, std::memory_order::seq_cst));

        return old;
    }

    std::array<T, N> buf;
    volatile size_t range_end;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};
