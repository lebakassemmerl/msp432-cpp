// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#include "cm4f.h"
#include "gpio.h"
#include "msp432.h"

extern void unhandled_interrupt(void);

void systick_handler(void) noexcept
{
    Msp432::instance().cortexm4f().systick().handle_interrupt();
}

void hard_fault(void) noexcept
{
    // we don't care what happened before, we initialize a new LED pin
    Pin led_red = Msp432::instance().gpio_pins().int_pin(IntPinNr::P01_0);
    led_red.make_output();
    led_red.set_high();

    while (true) {
        for (uint32_t i = 0; i < 1000 * 1000; i++)
            asm("nop");

        led_red.toggle();
    }
}

void fpu_handler(void) noexcept
{
    constexpr size_t FLOAT_IRQ_NR = 4; // see startup.cpp
    Msp432::instance().cortexm4f().nvic().clear_pending(FLOAT_IRQ_NR);
    Msp432::instance().cortexm4f().fpu().handle_interrupt();
}

void periph_int_handler(void) noexcept
{
    Msp432& msp = Msp432::instance();
    Nvic& nvic = msp.cortexm4f().nvic();
    uint64_t ints = nvic.get_pending_interrupts();
    nvic.clear_all_pending();

    // TODO: introduce a constant for the number of interrupts
    for (int i = 0; i < 64; i++) {
        uint64_t bit = 1ULL << i;
        if (!(bit & ints))
            continue;

        // interrupt occurred -> forward it to the appropriate handler
        switch (i) {
        case 16: msp.uscia0().handle_interrupt(); break;
        case 17: msp.uscia1().handle_interrupt(); break;
        case 18: msp.uscia2().handle_interrupt(); break;
        case 19: msp.uscia3().handle_interrupt(); break;
        case 20: msp.uscib0().handle_interrupt(); break;
        case 21: msp.uscib1().handle_interrupt(); break;
        case 22: msp.uscib2().handle_interrupt(); break;
        case 23: msp.uscib3().handle_interrupt(); break;
        case 30: msp.dma().handle_interrupt(-1); break; // error interrupt
        case 31: msp.dma().handle_interrupt(0); break;
        case 32: msp.dma().handle_interrupt(1); break;
        case 33: msp.dma().handle_interrupt(2); break;
        case 34: msp.dma().handle_interrupt(3); break;

        default:
            // TODO: implement better way for handling this case
            unhandled_interrupt();
            break;
        }
    }
}