// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <expected>
#include <span>

#include "button.h"
#include "cs.h"
#include "cortexm4f.h"
#include "event_timer.h"
#include "gpio.h"
#include "helpers.h"
#include "led.h"
#include "msp432.h"
#include "pin.h"
#include "wdt.h"

Msp432& chip = Msp432::instance();
EventTimer ev_timer{chip.t32_1()};

Led led_red = Led{chip.gpio_pins().int_pin(IntPinNr::P02_0), false};
Led led_green = Led{chip.gpio_pins().int_pin(IntPinNr::P02_1), false};
Led led_blue = Led{chip.gpio_pins().int_pin(IntPinNr::P02_2), false};
Button btn1{chip.gpio_pins().int_pin(IntPinNr::P01_1), ev_timer, true};

static void btn_cb(ButtonEvent ev, void* cookie) noexcept
{
    switch (ev) {
    case ButtonEvent::Pressed: led_blue.on(); break;
    case ButtonEvent::Released: led_blue.off(); break;
    case ButtonEvent::LongPress: led_green.toggle(); break;
    default: break;
    }
}

int main(void)
{
    chip.init();


    led_red.init();
    led_green.init();
    led_blue.init();

    ev_timer.init(chip.cs());
    btn1.init(nullptr, btn_cb);
    btn1.enable_event(ButtonEvent::Pressed);
    btn1.enable_event(ButtonEvent::Released);
    btn1.enable_event(ButtonEvent::LongPress);

    while (true) {
        //led_green.toggle();
        chip.delay_ms(500);
    }
}
