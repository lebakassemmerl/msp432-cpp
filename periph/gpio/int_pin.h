// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include "pin.h"

enum class IntPinNr : uint8_t {
    P01_0, P01_1, P01_2, P01_3, P01_4, P01_5, P01_6, P01_7,
    P02_0, P02_1, P02_2, P02_3, P02_4, P02_5, P02_6, P02_7,
    P03_0, P03_1, P03_2, P03_3, P03_4, P03_5, P03_6, P03_7,
    P04_0, P04_1, P04_2, P04_3, P04_4, P04_5, P04_6, P04_7,
    P05_0, P05_1, P05_2, P05_3, P05_4, P05_5, P05_6, P05_7,
    P06_0, P06_1, P06_2, P06_3, P06_4, P06_5, P06_6, P06_7,
};

class IntPin: public Pin {
public:
    constexpr ~IntPin() {};

    friend class GpioPins;
private:
    constexpr explicit IntPin(IntPinNr nr) noexcept 
        : Pin(static_cast<uint8_t>(nr) % PINS_PER_PORT,
        (static_cast<uint8_t>(nr) / PINS_PER_PORT) % 2,
        (static_cast<uint8_t>(nr) / PINS_PER_PORT) / 2) {}
};
