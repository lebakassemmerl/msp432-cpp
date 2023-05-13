// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 *
 * This file was inspired by / adapted from the linux-kernel:
 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/arm/include/asm/atomic.h)
 */

#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>

#define ATOMIC_OP_CONSTEXPR(asmop, operand, load, store) \
    __asm__ __volatile__( \
        "1: " #load  " %0, [%3]\n" \
              #asmop " %0, %0, #" #operand "\n" \
              #store " %1, %0, [%3]\n" \
              "teq     %1, #0\n" \
              "bne     1b" \
        : "=&r" (result), "=&r" (tmp), "+Qo" (this->val) \
        : "r" (&this->val) \
        : "cc")

#define ATOMIC_OP(asmop, value, load, store) \
    __asm__ __volatile__( \
        "1: " #load  " %0, [%3]\n" \
              #asmop " %0, %0, %4\n" \
              #store " %1, %0, [%3]\n" \
              "teq     %1, #0\n" \
              "bne     1b" \
        : "=&r" (result), "=&r" (tmp), "+Qo" (this->val) \
        : "r" (&this->val), "Ir" (value) \
        : "cc")

template<typename T>
concept AtomicType = (
    (std::same_as<uint8_t, T> || std::same_as<int8_t, T> ||
    std::same_as<uint16_t, T> || std::same_as<int16_t, T> ||
    std::same_as<uint32_t, T> || std::same_as<int32_t, T> || std::same_as<size_t, T>) &&
    ((sizeof(T) == 1) || (sizeof(T) == 2) || (sizeof(T) == 4))
);

template<typename T> requires AtomicType<T>
class Atomic {
public:
    constexpr explicit Atomic() noexcept : val(0) {}
    constexpr explicit Atomic(T val) noexcept : val(val) {}

    T get() const noexcept { return val; }
    void set(T value) noexcept { val = value; }


    // let prefix- and postfix-operator return the same value, namely the new one
    inline T operator++(int) noexcept { return operator++(); } // postfix
    inline T operator++() noexcept // prefix
    {
        T tmp;
        T result;

        if constexpr (sizeof(T) == 1) {
            ATOMIC_OP_CONSTEXPR(add, 1, ldrexb, strexb);
        } else if constexpr (sizeof(T) == 2) {
            ATOMIC_OP_CONSTEXPR(add, 1, ldrexh, strexh);
        } else if constexpr (sizeof(T) == 4) {
            ATOMIC_OP_CONSTEXPR(add, 1, ldrex, strex);
        }

        return result;
    }

    // let prefix- and postfix-operator return the same value, namely the new one
    inline T operator--(int) noexcept { return operator--(); } // posfix
    inline T operator--() noexcept // prefix
    {
        T tmp;
        T result;

        if constexpr (sizeof(T) == 1) {
            ATOMIC_OP_CONSTEXPR(sub, 1, ldrexb, strexb);
        } else if constexpr (sizeof(T) == 2) {
            ATOMIC_OP_CONSTEXPR(sub, 1, ldrexh, strexh);
        } else if constexpr (sizeof(T) == 4) {
            ATOMIC_OP_CONSTEXPR(sub, 1, ldrex, strex);
        }

        return result;
    }

    inline T operator+=(T v) noexcept
    {
        T tmp;
        T result;

        if constexpr (sizeof(T) == 1) {
            ATOMIC_OP(add, v, ldrexb, strexb);
        } else if constexpr (sizeof(T) == 2) {
            ATOMIC_OP(add, v, ldrexh, strexh);
        } else if constexpr (sizeof(T) == 4) {
            ATOMIC_OP(add, v, ldrex, strex);
        }

        return result;
    }

    inline T operator-=(T v) noexcept
    {
        T tmp;
        T result;

        if constexpr (sizeof(T) == 1) {
            ATOMIC_OP(sub, v, ldrexb, strexb);
        } else if constexpr (sizeof(T) == 2) {
            ATOMIC_OP(sub, v, ldrexh, strexh);
        } else if constexpr (sizeof(T) == 4) {
            ATOMIC_OP(sub, v, ldrex, strex);
        }

        return result;
    }

private:
    volatile T val;
};

#undef ATOMIC_OP_CONSTEXPR
#undef ATOMIC_OP
