// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <atomic>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <thread>

#include "../core/dma_fifo.h"

constexpr uint64_t MAX_VAL = 60000000;
constexpr size_t SIZE = 128;

DmaFifo<uint64_t, SIZE> fifo{};

void producer()
{
    static uint64_t buf[fifo.size() + 1];
    uint64_t i = 0;
    size_t avail;

    std::cout << "producer thread started" << std::endl;
    while (i < MAX_VAL) {
        avail = fifo.free();
        if (avail == 0)
            continue;

        for (size_t j = 0; j < avail; j++, i++)
            buf[j] = i;

        fifo.put_range(std::span{buf, avail});
    }

    std::cout << "producer thread finished" << std::endl;
}

void consumer()
{
    bool empty;
    uint64_t compare = 0;
    std::span<uint64_t> data;

    std::cout << "consumer thread started" << std::endl;
    while (true) {
        if (fifo.is_empty())
            continue;

        data = fifo.peek_range().value();

        for (size_t i = 0; i < data.size(); i++, compare++) {
            if (data[i] != compare) {
                std::cout << "consumer: entry: 0x" << std::hex << data[i] << ", expected: 0x"
                << compare << ", diff: " << static_cast<int64_t>(data[i] - compare) << std::endl;
            }
        }

        fifo.drop_range();

        if (compare >= (MAX_VAL - 1))
            break;
    }

    std::cout << "consumer thread finished" << std::endl;
}

int main(void)
{
    std::cout << "Start test of core/dma_fifo.h" << std::endl;

    std::thread t1{producer};
    std::thread t2{consumer};

    t1.join();
    t2.join();

    return 0;
}
