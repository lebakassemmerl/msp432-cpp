// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <array>
#include <cstddef>

#include "pin.h"
#include "int_pin.h"

class GpioPins {
public:
    static constexpr size_t INT_PIN_CNT = 48;
    static constexpr size_t PIN_CNT = 40;

    GpioPins(const GpioPins&) = delete;
    GpioPins(const GpioPins&&) = delete;
    GpioPins& operator=(const GpioPins&) = delete;
    GpioPins& operator=(const GpioPins&&) = delete;
    constexpr ~GpioPins() {};

    constexpr Pin& pin(PinNr nr) noexcept
    {
        return pins[static_cast<size_t>(nr)];
    }
    
    constexpr IntPin& int_pin(IntPinNr nr) noexcept
    {
        return int_pins[static_cast<size_t>(nr)];
    }

    friend void init_platform(void); // from startup.cpp
    friend class Msp432;
private:
    constexpr explicit GpioPins() noexcept : int_pins {
        IntPin{IntPinNr::P01_0}, IntPin{IntPinNr::P01_1}, IntPin{IntPinNr::P01_2}, IntPin{IntPinNr::P01_3},
        IntPin{IntPinNr::P01_4}, IntPin{IntPinNr::P01_5}, IntPin{IntPinNr::P01_6}, IntPin{IntPinNr::P01_7},
        IntPin{IntPinNr::P02_0}, IntPin{IntPinNr::P02_1}, IntPin{IntPinNr::P02_2}, IntPin{IntPinNr::P02_3},
        IntPin{IntPinNr::P02_4}, IntPin{IntPinNr::P02_5}, IntPin{IntPinNr::P02_6}, IntPin{IntPinNr::P02_7},
        IntPin{IntPinNr::P03_0}, IntPin{IntPinNr::P03_1}, IntPin{IntPinNr::P03_2}, IntPin{IntPinNr::P03_3},
        IntPin{IntPinNr::P03_4}, IntPin{IntPinNr::P03_5}, IntPin{IntPinNr::P03_6}, IntPin{IntPinNr::P03_7},
        IntPin{IntPinNr::P04_0}, IntPin{IntPinNr::P04_1}, IntPin{IntPinNr::P04_2}, IntPin{IntPinNr::P04_3},
        IntPin{IntPinNr::P04_4}, IntPin{IntPinNr::P04_5}, IntPin{IntPinNr::P04_6}, IntPin{IntPinNr::P04_7},
        IntPin{IntPinNr::P05_0}, IntPin{IntPinNr::P05_1}, IntPin{IntPinNr::P05_2}, IntPin{IntPinNr::P05_3},
        IntPin{IntPinNr::P05_4}, IntPin{IntPinNr::P05_5}, IntPin{IntPinNr::P05_6}, IntPin{IntPinNr::P05_7},
        IntPin{IntPinNr::P06_0}, IntPin{IntPinNr::P06_1}, IntPin{IntPinNr::P06_2}, IntPin{IntPinNr::P06_3},
        IntPin{IntPinNr::P06_4}, IntPin{IntPinNr::P06_5}, IntPin{IntPinNr::P06_6}, IntPin{IntPinNr::P06_7}
        }, pins {
        Pin{PinNr::P07_0}, Pin{PinNr::P07_1}, Pin{PinNr::P07_2}, Pin{PinNr::P07_3},
        Pin{PinNr::P07_4}, Pin{PinNr::P07_5}, Pin{PinNr::P07_6}, Pin{PinNr::P07_7},
        Pin{PinNr::P08_0}, Pin{PinNr::P08_1}, Pin{PinNr::P08_2}, Pin{PinNr::P08_3},
        Pin{PinNr::P08_4}, Pin{PinNr::P08_5}, Pin{PinNr::P08_6}, Pin{PinNr::P08_7},
        Pin{PinNr::P09_0}, Pin{PinNr::P09_1}, Pin{PinNr::P09_2}, Pin{PinNr::P09_3},
        Pin{PinNr::P09_4}, Pin{PinNr::P09_5}, Pin{PinNr::P09_6}, Pin{PinNr::P09_7},
        Pin{PinNr::P10_0}, Pin{PinNr::P10_1}, Pin{PinNr::P10_2}, Pin{PinNr::P10_3},
        Pin{PinNr::P10_4}, Pin{PinNr::P10_5}, Pin{PinNr::P10_6}, Pin{PinNr::P10_7},
        Pin{PinNr::PJ_0},  Pin{PinNr::PJ_1},  Pin{PinNr::PJ_2},  Pin{PinNr::PJ_3},
        Pin{PinNr::PJ_4},  Pin{PinNr::PJ_5},  Pin{PinNr::PJ_6},  Pin{PinNr::PJ_7}} {}

    std::array<IntPin, INT_PIN_CNT> int_pins;
    std::array<Pin, PIN_CNT> pins;
};
