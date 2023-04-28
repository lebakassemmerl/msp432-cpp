// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include <span>

#include "helpers.h"
#include "msp432.h"
#include "cortexm4f.h"
#include "gpio.h"
#include "pin.h"
#include "cs.h"
#include "spi.h"
#include "spi_master.h"
#include "uart.h"
#include "wdt.h"

#define charptr_to_span(ptr) \
    std::span{reinterpret_cast<uint8_t*>(const_cast<char*>(ptr)), ARRAY_SIZE(ptr) - 1}

Msp432& chip = Msp432::instance();
Uart uart0{chip.uscia0(), chip.dma(), 115200, 0, 1, 1, 1};
SpiMaster spi1{chip.uscib1(), chip.dma(), SpiMode::Cpol0Cphase0, 1000000, 2, 3, 2, 2};

int main(void)
{
    chip.init();

    // UART0 pins
    chip.gpio_pins().int_pin(IntPinNr::P01_2).enable_primary_function();
    chip.gpio_pins().int_pin(IntPinNr::P01_3).enable_primary_function();

    uart0.init(chip.cs());

    auto& led_red = chip.gpio_pins().int_pin(IntPinNr::P02_0);
    auto& led_green = chip.gpio_pins().int_pin(IntPinNr::P02_1);
    auto& led_blue = chip.gpio_pins().int_pin(IntPinNr::P02_2);
    auto& btn1 = chip.gpio_pins().int_pin(IntPinNr::P01_1);

    led_red.make_output();
    led_green.make_output();
    led_blue.make_output();
    led_red.set_low();
    led_green.set_low();
    led_blue.set_high();

    btn1.make_input();
    btn1.set_pull_mode(PullMode::PullUp);

    uart0.write(charptr_to_span("Hallo erstmal!\r\n"));
    while (true) {
        if (!btn1.read())
            led_blue.toggle();

        led_green.toggle();
        uart0.write(charptr_to_span("test write\r\n"));
        chip.delay_ms(500);
    }
}
