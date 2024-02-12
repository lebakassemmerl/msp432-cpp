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
#include "w2812b.h"
#include "wdt.h"

Msp432& chip = Msp432::instance();
Uart uart0{chip.uscia0(), chip.dma(), 115200, 0, 1, 1, 1};
SpiMaster spi1{chip.uscib1(), chip.dma(), SpiMode::Cpol0Cphase0, 6'000'000, 2, 3, 2, 2};
W2812B<100> ledstrip{spi1};

int main(void)
{
    uint8_t green = 0;
    chip.init();

    // UART0 pins
    chip.gpio_pins().int_pin(IntPinNr::P01_2).enable_primary_function();
    chip.gpio_pins().int_pin(IntPinNr::P01_3).enable_primary_function();

    uart0.init(chip.cs());
    spi1.init(chip.cs());

    Led led_red = Led{chip.gpio_pins().int_pin(IntPinNr::P02_0), false};
    Led led_green = Led{chip.gpio_pins().int_pin(IntPinNr::P02_1), false};
    Led led_blue = Led{chip.gpio_pins().int_pin(IntPinNr::P02_2), false};
    Pin& btn1 = chip.gpio_pins().int_pin(IntPinNr::P01_1);

    led_red.init();
    led_green.init();
    led_blue.init();

    btn1.make_input();
    btn1.set_pull_mode(PullMode::PullUp);

    uart0.write("\r\nHallo erstmal!\r\n");
    ledstrip.init();
    ledstrip.set_color_for_all_leds(Rgb{0x3A, 0xC5, 0x81});
    ledstrip.refresh_leds();

    while (true) {
        if (!btn1.read())
            led_blue.toggle();

        ledstrip.set_color_for_all_leds(Rgb{0, green, 0});
        ledstrip.refresh_leds();

        green += 10;
        led_green.toggle();
        uart0.write("\r\nTest write.");
        chip.delay_ms(500);
    }
}
