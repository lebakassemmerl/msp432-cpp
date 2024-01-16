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
#include <cstddef>
#include <expected>
#include <span>

#include "err.h"
#include "helpers.h"
#include "libc.h"

template<typename T, size_t N>
class DmaFifo {
public:
    static_assert(hlp::is_powerof2<N>(), "N must be power of 2");

    constexpr explicit DmaFifo() noexcept : buf(), range_end(0), head(0), tail(0) {}
    constexpr ~DmaFifo() {}

    // must only be called from the producer!
    Err put_range(std::span<T> data) noexcept
    {
        size_t oldhead;
        size_t nw;
        size_t h;

        if (free() < data.size())
            return Err::NoMem;

        // first, we increment 'tmphead' in order to reserve the amount of bytes we need
        h = tmphead.load(std::memory_order::relaxed);
        do {
            nw = (h + data.size()) & (N - 1);
        } while (!tmphead.compare_exchange_weak(h, nw, std::memory_order::seq_cst));

        if ((h + data.size()) > N) {
            size_t tmp = N - h;
            libc::memcpy(&buf[h], &data[0], tmp * sizeof(data[0]));
            libc::memcpy(&buf[0], &data[tmp], (data.size() - tmp) * sizeof(data[0]));
        } else {
            libc::memcpy(&buf[h], data.data(), data.size() * sizeof(data[0]));
        }

        // check if we are the application or the interrupt-thread
        oldhead = head.load(std::memory_order::seq_cst);
        if (oldhead == h) {
            // we are the 1st / only thread -> we have to update 'head' accordingly
            nw = tmphead.load(std::memory_order::seq_cst);
            head.store(nw, std::memory_order::seq_cst);
        }

        // if (oldhead != h): we are the interrupt-thread -> we don't have to do anything here

        return Err::Ok;
    }

    // must only be called from the consumer!
    std::expected<std::span<T>, Err> peek_range() noexcept
    {
        size_t h;
        size_t t;

        if (is_empty())
            return std::unexpected{Err::Empty};

        // load head first, since we are the consumer
        h = head.load(std::memory_order::seq_cst);
        t = tail.load(std::memory_order::seq_cst);

        if (h > t) {
            range_end = h; // the currently returned range ends where head is right now
            return std::expected<std::span<T>, Err>{std::span<T>{&buf[t], h - t}};
        } else {
            // The contiguous memory ends at the end of the buffer -> return everything from tail to
            // the end of the buffer.
            range_end = 0;
            return std::expected<std::span<T>, Err>{std::span<T>{&buf[t], N - t}};
        }
    }

    // must only be called from the consumer!
    void drop_range() noexcept { tail.store(range_end, std::memory_order::seq_cst); }

    inline bool is_full() const noexcept { return used() == (N - 1); }
    inline bool is_empty() const noexcept { return used() == 0; }
    inline size_t free() const noexcept { return (N - 1 - used()); }
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

    constexpr size_t size() const noexcept { return N - 1; }
private:

    std::array<T, N> buf;
    volatile size_t range_end;
    std::atomic<size_t> tmphead;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};
