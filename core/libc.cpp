// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2024
 * E-Mail: hotschi@gmx.at
 */

#include <cstddef>
#include <cstdint>

#include "libc.h"

extern void hard_fault(void);

namespace libc {
extern "C" {
    size_t strlen(const char* str)
    {
        size_t len = 0;

        while (*str) {
            str++;
            len++;
        }

        return len;
    }
}
}