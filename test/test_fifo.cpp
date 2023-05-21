// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <thread>

#include "../core/fifo.h"

constexpr size_t STR_SIZE = 64;
constexpr uint64_t MAX_VAL = 0x1000000;

template<size_t N>
struct MyStr {
    constexpr explicit MyStr() noexcept {}
    constexpr size_t size() const noexcept { return N; }
    char buf[N];
};

template<size_t N>
struct Entry {
    constexpr explicit Entry() noexcept : u64(0), u32(0), u16(0) {}
    constexpr explicit Entry(uint64_t v) noexcept
        : u64(v), u32(static_cast<uint32_t>(v)), u16(static_cast<uint16_t>(v)) {}

    inline void incr() noexcept { u64++; u32++; u16++; }
    inline MyStr<N> to_string() noexcept
    {
        MyStr<N> str{};
        snprintf(str.buf, str.size(), "u64: 0x%16llx, u32: 0x%08x, u16: 0x%04x\n", u64, u32, u16);

        return str;
    }
    uint64_t u64;
    uint32_t u32;
    uint16_t u16;
};

Fifo<Entry<STR_SIZE>, 128> fifo{};

void producer()
{
    uint64_t i = 0;

    std::cout << "producer thread started" << std::endl;
    while (i < MAX_VAL) {
        if (fifo.is_full())
            continue;

        Entry<STR_SIZE> e{i};
        fifo.push(e);
        i++;
    }

    std::cout << "producer thread finished" << std::endl;
}

void consumer()
{
    uint64_t compare = 0;

    std::cout << "consumer thread started" << std::endl;
    while (true) {
        if (fifo.is_empty()) {
            continue;
        }

        Entry<STR_SIZE> e = fifo.pop_elem().value();
        // std::cout << e.to_string().buf;
        // f << e.to_string().buf;
        if (e.u64 != compare) {
            std::cout << "consumer: entry: 0x" << std::hex << e.u64 << ", expected: 0x"
            << compare << ", diff: "
            << (static_cast<int64_t>(e.u64) - static_cast<int64_t>(compare)) << std::endl;
        }

        compare++;
        if (e.u64 >= (MAX_VAL - 1))
            break;
    }

    std::cout << "consumer thread finished" << std::endl;
}

int main(void)
{
    std::cout << "Start test of core/fifo.h" << std::endl;
    std::thread t1{producer};
    std::thread t2{consumer};

    t1.join();
    t2.join();

    return 0;
}
