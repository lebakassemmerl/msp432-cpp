// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <array>
#include <expected>
#include <functional>
#include <cstddef>
#include <optional>
#include <utility>

#include "err.h"

template<typename T, size_t N>
class Fifo {
public:
    constexpr Fifo() noexcept : used_elems(0), head(0), tail(0) {}
    constexpr ~Fifo() noexcept {}

    err::Err push(const T& val) noexcept
    {
        if (used_elems >= N)
            return err::Err::NoMem;

        buf[head] = val;
        inc_used();

        return err::Err::Ok;
    }

    template<typename... Args>
    err::Err emplace(Args&&... args) noexcept
    {
        if (used_elems >= N)
            return err::Err::NoMem;

        new(&buf[head])T{std::forward<Args>(args)...};
        inc_used();

        return err::Err::Ok;
    }

    std::expected<T, err::Err> peek() noexcept
    {
        if (used_elems == 0)
            return std::unexpected{err::Err::Empty};

        return std::expected<T, err::Err>{buf[tail]};
    }

    std::expected<std::reference_wrapper<T>, err::Err> peek_ref() noexcept
    {
        if (used_elems == 0)
            return std::unexpected{err::Err::Empty};

        return std::expected<std::reference_wrapper<T>, err::Err>{
            std::reference_wrapper<T>{buf[tail]}};
    }

    std::expected<T, err::Err> pop_elem() noexcept
    {
        if (used_elems == 0)
            return std::unexpected{err::Err::Empty};

        T ret = buf[tail];
        dec_used();

        return std::expected<T, err::Err>{ret};
    }

    err::Err pop() noexcept
    {
        if (used_elems == 0)
            return err::Err::Empty;

        dec_used();
        return err::Err::Ok;
    }

    size_t used() const noexcept { return used_elems; }
    size_t free() const noexcept { return N - used_elems; }
    bool is_full() const noexcept { return used_elems == N; }
    bool is_empty() const noexcept { return used_elems == 0; }

private:
    inline void inc_used() noexcept
    {
        used_elems++;
        head = head + 1;
        if (head >= N)
            head = 0;
    }

    inline void dec_used() noexcept
    {
        used_elems--;
        tail = tail + 1;
        if (tail >= N)
            tail = 0;
    }

    std::array<T, N> buf;
    size_t used_elems;
    volatile size_t head;
    volatile size_t tail;
};