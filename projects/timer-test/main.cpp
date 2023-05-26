// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <expected>
#include <span>

#include "cs.h"
#include "cortexm4f.h"
#include "event_timer.h"
#include "fifo.h"
#include "gpio.h"
#include "helpers.h"
#include "led.h"
#include "msp432.h"
#include "pin.h"
#include "uart.h"
#include "wdt.h"

#define charptr_to_span(ptr) \
    std::span{reinterpret_cast<uint8_t*>(const_cast<char*>(ptr)), ARRAY_SIZE(ptr) - 1}

Msp432& chip = Msp432::instance();
Uart uart0{chip.uscia0(), chip.dma(), 115200, 0, 1, 1, 1};
EventTimer ev_timer{chip.t32_1()};

static void ev_cb(void* cookie) noexcept
{
    Led* led = reinterpret_cast<Led*>(cookie);

    led->toggle();
}

int main(void)
{
    chip.init();

    // UART0 pins
    chip.gpio_pins().int_pin(IntPinNr::P01_2).enable_primary_function();
    chip.gpio_pins().int_pin(IntPinNr::P01_3).enable_primary_function();

    uart0.init(chip.cs());
    Led led_red = Led(chip.gpio_pins().int_pin(IntPinNr::P02_0), false);
    Led led_green = Led(chip.gpio_pins().int_pin(IntPinNr::P02_1), false);
    Led led_blue = Led(chip.gpio_pins().int_pin(IntPinNr::P02_2), false);

    led_red.init();
    led_green.init();
    led_blue.init();

    ev_timer.init(chip.cs());
    auto event250 = ev_timer.register_event(250, &led_red, ev_cb).value();
    auto event125 = ev_timer.register_event(125, &led_blue, ev_cb).value();
    ev_timer.start_event(event250);
    ev_timer.start_event(event125);

    uart0.write(charptr_to_span("\r\nTimer test\r\n"));
    while (true) {
        //led_green.toggle();
        chip.delay_ms(500);
    }
}
