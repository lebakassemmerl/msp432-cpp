// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>
#include <span>

#include "usci.h"

enum class SpiMode : uint16_t {
    Cpol0Cphase0 = 0,
    Cpol1Cphase0 = 1,
    Cpol0Cphase1 = 2,
    Cpol1Cphase1 = 3,
};

enum class SpiTransferType : uint8_t {
    None,
    Write,
    Read,
    WriteRead,
};

typedef void (*SpiCallback)(
    SpiTransferType type,
    std::span<uint8_t> txbuf,
    std::span<uint8_t> rxbuf,
    void* context);
