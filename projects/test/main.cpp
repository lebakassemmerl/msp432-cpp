// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include <span>

#include "cs.h"
#include "cortexm4f.h"
#include "fifo.h"
#include "gpio.h"
#include "helpers.h"
#include "led.h"
#include "msp432.h"
#include "pin.h"
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
    spi1.init(chip.cs());

    Led led_red = Led(chip.gpio_pins().int_pin(IntPinNr::P02_0), false);
    Led led_green = Led(chip.gpio_pins().int_pin(IntPinNr::P02_1), false);
    Led led_blue = Led(chip.gpio_pins().int_pin(IntPinNr::P02_2), false);
    Pin& btn1 = chip.gpio_pins().int_pin(IntPinNr::P01_1);

    led_red.init();
    led_green.init();
    led_blue.init();

    btn1.make_input();
    btn1.set_pull_mode(PullMode::PullUp);

    uart0.write(charptr_to_span("\r\nHallo erstmal!\r\n"));
    while (true) {
        if (!btn1.read())
            led_blue.toggle();

        led_green.toggle();
        uart0.write(charptr_to_span("\r\nTest write."));
        chip.delay_ms(500);
    }
}
