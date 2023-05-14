// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
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

template<typename T, size_t N>
class Fifo {
public:
    constexpr Fifo() noexcept : used_elems(0), head(0), tail(0) {}
    constexpr ~Fifo() noexcept {}

    err::Err push(const T& val) noexcept
    {
        if (used_elems.load() >= N)
            return err::Err::NoMem;

        buf[head.load()] = val;
        inc_used();

        return err::Err::Ok;
    }

    template<typename... Args>
    err::Err emplace(Args&&... args) noexcept
    {
        if (used_elems.load() >= N)
            return err::Err::NoMem;

        new(&buf[head.load()])T{std::forward<Args>(args)...};
        inc_used();

        return err::Err::Ok;
    }

    std::expected<T, err::Err> peek() noexcept
    {
        if (used_elems.load() == 0)
            return std::unexpected{err::Err::Empty};

        return std::expected<T, err::Err>{buf[tail.load()]};
    }

    std::expected<std::reference_wrapper<T>, err::Err> peek_ref() noexcept
    {
        if (used_elems.load() == 0)
            return std::unexpected{err::Err::Empty};

        return std::expected<std::reference_wrapper<T>, err::Err>{
            std::reference_wrapper<T>{buf[tail.load()]}};
    }

    std::expected<T, err::Err> pop_elem() noexcept
    {
        if (used_elems.load() == 0)
            return std::unexpected{err::Err::Empty};

        T ret = buf[tail.load()];
        dec_used();

        return std::expected<T, err::Err>{ret};
    }

    err::Err pop() noexcept
    {
        if (used_elems.load() == 0)
            return err::Err::Empty;

        dec_used();
        return err::Err::Ok;
    }

    size_t used() const noexcept { return used_elems.load(); }
    size_t free() const noexcept { return N - used_elems.load(); }
    bool is_full() const noexcept { return used_elems.load() == N; }
    bool is_empty() const noexcept { return used_elems.load() == 0; }

private:
    inline void inc_used() noexcept
    {
        used_elems++;
        if (++head >= N)
            head.store(0);
    }

    inline void dec_used() noexcept
    {
        used_elems--;
        if (++tail >= N)
            tail.store(0);
    }

    std::array<T, N> buf;
    std::atomic<size_t> used_elems;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};