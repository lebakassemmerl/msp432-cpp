// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include <cstddef>
#include <cstdint>

#include "libc.h"

extern void hard_fault(void);

namespace libc {
extern "C" {
    void* memset(void* str, uint8_t val, size_t nr_bytes)
    {
        constexpr int INT_SIZE = sizeof(unsigned);

        unsigned val_big = 0;
        uint8_t* str_small;
        size_t i;

        for (int i = 0; i < INT_SIZE; i++)
            val_big += static_cast<unsigned>(val) << (i * 8);

        for (i = 0; i < (nr_bytes / INT_SIZE); i++)
            reinterpret_cast<uint32_t*>(str)[i] = val_big;

        str_small = &reinterpret_cast<uint8_t*>(str)[INT_SIZE * i];
        for (i = 0; i < (nr_bytes & (INT_SIZE - 1)); i++)
            str_small[i] = val;

        return str;
    }

    void* memcpy(void* dst, const void* src, size_t nr_bytes)
    {
        constexpr int INT_SIZE = sizeof(unsigned);

        size_t i;
        uint8_t* dst_small;
        const uint8_t* src_small;

        for (i = 0; i < (nr_bytes / INT_SIZE); i++)
            reinterpret_cast<unsigned*>(dst)[i] = reinterpret_cast<const unsigned*>(src)[i];

        dst_small = &reinterpret_cast<uint8_t*>(dst)[INT_SIZE * i];
        src_small = &reinterpret_cast<const uint8_t*>(src)[INT_SIZE * i];
        
        for (i = 0; i < (nr_bytes & (INT_SIZE - 1)); i++)
            dst_small[i] = src_small[i];

        return dst;
    }

    size_t strlen(const char* str)
    {
        size_t len = 0;

        while (*str) {
            str++;
            len++;
        }

        return len;
    }

    void __cxa_pure_virtual()
    {
        // Gets called if a pure virtual function (without override) is invoked.
        // FIXME: for now, simply do nothing
    }

    // we are not dealing with threads but since we have no std-lib we need this function call
    // for destructors of global instances
    int atexit(void (*func)(void))
    {
        return 0;
    }

    void abort(void)
    {
        hard_fault();
    }
}
}