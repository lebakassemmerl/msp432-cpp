// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <array>
#include <functional>
#include <cstddef>
#include <optional>
#include <utility>

#include "err.h"

template<typename T, size_t N>
class Fifo {
public:
    constexpr Fifo() noexcept : m_used(0), m_head(0), m_tail(0) {}
    constexpr ~Fifo() noexcept {}

    err::Err push(const T& val) noexcept
    {
        if (m_used >= N)
            return err::Err::NoMem;

        buf[m_head] = val;
        inc_used();

        return err::Err::Ok;
    }

    std::pair<err::Err, std::optional<std::reference_wrapper<T>>> emplace() noexcept
    {
        if (m_used >= N)
            return {err::Err::NoMem, std::nullopt};

        T& ref = buf[m_head];
        inc_used();

        return {err::Err::Ok, ref};
    }

    std::pair<err::Err, std::optional<std::reference_wrapper<T>>> peek() noexcept
    {
        if (m_used == 0)
            return {err::Err::Empty, std::nullopt};

        return {err::Err::Ok, buf[m_tail]};
    }

    std::pair<err::Err, std::optional<std::reference_wrapper<T>>> pop_elem() noexcept
    {
        if (m_used == 0)
            return {err::Err::Empty, std::nullopt};

        T& ref = buf[m_tail];
        dec_used();

        return {err::Err::Ok, ref};
    }

    err::Err pop() noexcept
    {
        if (m_used == 0)
            return err::Err::Empty;

        dec_used();
        return err::Err::Ok;
    }

    size_t used() const noexcept { return m_used; }
    size_t free() const noexcept { return N - m_used; }
    bool is_full() const noexcept { return m_used == N; }
    bool is_empty() const noexcept { return m_used == 0; }

private:
    inline void inc_used() noexcept
    {
        m_used++;
        m_head = m_head + 1;
        if (m_head >= N)
            m_head = 0;
    }

    inline void dec_used() noexcept
    {
        m_used--;
        m_tail = m_tail + 1;
        if (m_tail >= N)
            m_tail = 0;
    }

    std::array<T, N> buf;
    size_t m_used;
    volatile size_t m_head;
    volatile size_t m_tail;
};