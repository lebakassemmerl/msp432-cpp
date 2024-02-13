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
#include "nt7108c.h"
#include "pin.h"
#include "spi.h"
#include "spi_master.h"
#include "uart.h"
#include "w2812b.h"
#include "wdt.h"

Msp432& chip = Msp432::instance();

Led led_red = Led{chip.gpio_pins().int_pin(IntPinNr::P02_0), false};
Led led_green = Led{chip.gpio_pins().int_pin(IntPinNr::P02_1), false};
Led led_blue = Led{chip.gpio_pins().int_pin(IntPinNr::P02_2), false};

Uart uart0{chip.uscia0(), chip.dma(), 115200, 0, 1, 1, 1};
SpiMaster spi1{chip.uscib1(), chip.dma(), SpiMode::Cpol1Cphase1, 200'000, 2, 3, 2, 2};
Lt7920 lcd{spi1, chip.gpio_pins().int_pin(IntPinNr::P04_1)};

int main(void)
{
    size_t idx = 0;
    chip.init();

    // UART0 pins
    chip.gpio_pins().int_pin(IntPinNr::P01_2).enable_primary_function();
    chip.gpio_pins().int_pin(IntPinNr::P01_3).enable_primary_function();

    // USCIB1 / SPI pins
    chip.gpio_pins().int_pin(IntPinNr::P06_3).enable_primary_function(); // clk
    chip.gpio_pins().int_pin(IntPinNr::P06_4).enable_primary_function(); // mosi
    chip.gpio_pins().int_pin(IntPinNr::P06_5).enable_primary_function(); // miso

    uart0.init(chip.cs());
    spi1.init(chip.cs());

    led_red.init();
    led_green.init();
    led_blue.init();

    led_blue.on();
    uart0.write("\r\nLCD-Test:\r\n");

    for (uint8_t y = 0; y < 64; y++) {
        for (uint8_t x = 0; x < 128; x++)
            lcd.set_pixel(x, y, static_cast<bool>(x & 0x01));
    }

    lcd.init();
    while (true) {
        // uart0.write("\r\nTest write.");
        // lcd.set_pixel(idx & 0x7F, (idx >> 7) & 0x3F, true);
        idx += 1;
        // chip.delay_ms(10);
    }
}
