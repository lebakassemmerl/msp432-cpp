// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <bit>
#include <concepts>
#include <cstdint>
#include <cstddef>

#include "helpers.h"

template<typename T> requires std::unsigned_integral<T>
class BitField {
public:
    constexpr explicit BitField<T>(int bit_high, int bit_low)
        : m(hlp::mask<T>(bit_high, bit_low)), v(0) {}

    constexpr explicit BitField<T>(T mask, T value) : m(mask), v(value) {}

    constexpr T mask() const { return m; }
    constexpr T get_value() const { return v; }
    constexpr BitField<T> value(T val) const
    {
        return BitField<T>(static_cast<T>(m), static_cast<T>(m & (val << std::countr_zero(m))));
    }

    constexpr T raw_value(T val) const
    {
        return m & (val << std::countr_zero(m));
    }

    friend constexpr BitField<T> operator+(BitField<T> lhs, const BitField<T>& rhs)
    {
        return BitField<T>(static_cast<T>(lhs.m | rhs.m), static_cast<T>(lhs.v | rhs.v));
    }

private:
    const T m;
    const T v;
};

template<typename T> requires std::unsigned_integral<T>
class ReadWrite {
public:
    ReadWrite() = delete;
    ReadWrite(T&) = delete;
    ReadWrite(T&&) = delete;
    ~ReadWrite() = delete;

    constexpr void set(T val) noexcept { reg = val; }
    constexpr void set(const BitField<T>& bf) { reg = bf.get_value(); }
    constexpr T get() const noexcept { return reg; }
    constexpr volatile T& get_ref() volatile noexcept { return reg; }

    constexpr void modify(const BitField<T>& b)
    {
        set((get() & ~b.mask()) | b.get_value());
    }

private:
    volatile T reg;
};

template<typename T> requires std::unsigned_integral<T>
class WriteOnly {
public:
    WriteOnly() = delete;
    WriteOnly(T&) = delete;
    WriteOnly(T&&) = delete;
    ~WriteOnly() = delete;

    constexpr void set(T val) noexcept { reg = val; }
    constexpr void set(const BitField<T>& bf) { reg = bf.get_value(); }

private:
    volatile T reg;
};

template<typename T> requires std::unsigned_integral<T>
class ReadOnly {
public:
    ReadOnly() = delete;
    ReadOnly(T&) = delete;
    ReadOnly(T&&) = delete;
    ~ReadOnly() = delete;

    constexpr T get() const noexcept { return reg; }

private:
    volatile T reg;
};

template<typename T> requires std::unsigned_integral<T>
class Reserved {
public:
    Reserved() = delete;
    Reserved(T&) = delete;
    Reserved(T&&) = delete;
    ~Reserved() = delete;

private:
    volatile T reg;
};

template<typename T> requires std::unsigned_integral<T>
class InMemory {
public:
    constexpr explicit InMemory(const T& val) noexcept : reg(val) {}
    InMemory(T&) = delete;

    constexpr void set(T val) noexcept { reg = val; }
    constexpr void set(const BitField<T>& bf) { reg = bf.get_value(); }
    constexpr T get() const noexcept { return reg; }
    constexpr volatile T& get_ref() volatile noexcept { return reg; }

    constexpr void modify(const BitField<T>& b)
    {
        set((get() & ~b.mask()) | b.get_value());
    }

private:
    volatile T reg;
};

// conept to ensure that the templated class provides the two member-functions
template<typename P>
concept PeripheralAccess =
    requires(P p) {
        p.before_peripheral_access();
        p.after_peripheral_access();
    };

// template class to simplify peripheral accesses when e.g. registers have to be unlocked before
// access and locked after the access
template<typename P> requires PeripheralAccess<P>
class PeripheralManager {
public:
    constexpr PeripheralManager(const P& p) noexcept : peripheral(p)
    {
        peripheral.before_peripheral_access();
    }

    constexpr ~PeripheralManager() noexcept
    {
        peripheral.after_peripheral_access();
    }

    inline const P& periph() const noexcept { return peripheral; }

private:
    const P& peripheral;
};
