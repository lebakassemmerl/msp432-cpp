// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 *
 * This file contains the startup code for the MSP432. It defines the interrupt vector tables for
 * the Cortex-M4F and the peripherals as well as the reset-handler which does the basic hardware
 * configuration in order to invoke the main-function.
 */

#include <cstdint>
#include <cstddef>

#include "libc.h"

#include "flctl.h"
#include "pcm.h"
#include "sysctl.h"
#include "wdt.h"

#define VECTOR_TABLE(x) __attribute__((section(x), used))

// defined in the linker-script
extern uint32_t _estack;
extern uint32_t _sstack;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern void (*_ctors_begin[])(void);
extern void (*_ctors_end[])(void);
extern void (*_dtors_begin[])(void);
extern void (*_dtors_end[])(void);

// entrance to the application
extern int main(void); 

// the individual interrupt handlers defined in the peripheral drivers
extern void systick_handler(void);
extern void fpu_handler(void);
extern void periph_int_handler(void);

void __attribute__((weak)) hard_fault(void)
{
    while (true)
        __asm__("nop");
}

void __attribute__((weak)) unhandled_interrupt(void)
{
    while (true)
        __asm__("nop");
}

static void do_constructors(void)
{
    size_t cnt = (_ctors_end - _ctors_begin) / sizeof(_ctors_begin[0]);
    for (size_t i = 0; i < cnt; i++)
        _ctors_begin[i]();
}

static void do_destructors(void)
{
    size_t cnt = (_dtors_end - _dtors_begin) / sizeof(_dtors_begin[0]);
    for (size_t i = 0; i < cnt; i++)
        _dtors_begin[i]();
}

static void init_ram(void)
{
    uint8_t* data = reinterpret_cast<uint8_t*>(&_sdata);
    uint8_t* text = reinterpret_cast<uint8_t*>(&_etext);
    size_t data_len = reinterpret_cast<size_t>(&_edata) - reinterpret_cast<size_t>(&_sdata);

    uint8_t* bss = reinterpret_cast<uint8_t*>(&_szero);
    size_t bss_len = reinterpret_cast<size_t>(&_ezero) - reinterpret_cast<size_t>(&_szero);

    // initialize the .data section
    libc::memcpy(data, text, data_len);
    
    // initialize the .bss section with 0
    libc::memset(bss, 0, bss_len);
}

// use a seperate function in order to destroy the objects on the stack before entering main
void init_platform(void)
{
    FlCtl flctl{};
    Pcm pcm{};
    SysCtl sysctl{};
    Wdt wdt{};

    wdt.disable();
    sysctl.enable_all_sram_banks();
    flctl.set_waitstates(WaitStates::_1);
    flctl.set_buffering(true);
    pcm.set_high_power();
}

void reset_handler(void)
{
    // first, initialize stack pointer
    __asm__("ldr sp, =_estack");

    // we have to do this before initializing the RAM, otherwise it can happen that the watchdog
    // resets the entires system because initializing the RAM hasn't finished yet
    init_platform();

    // after the stack pointer is initialized correctly, initialize the RAM 
    init_ram();

    // this should probably be done before touching the hardware peripherals, but since the watchdog
    // is enabled by default after reset, we just do it after the platform intialization
    do_constructors();

    // call the main function where the application is implemented
    (void)main();

    // not necessary since we will never reach this code but for the completeness sake..
    do_destructors();
}

// vector table for the ARM-specific interrupts
static VECTOR_TABLE(".arm_vector") void(*ARM_VECTOR[16])(void) = {
    reinterpret_cast<void(*)(void)>(&_estack),  // Beginning of stack
    reset_handler,          // Reset handler
    unhandled_interrupt,    // NMI
    hard_fault,             // Hard fault
    unhandled_interrupt,    // MemManage
    unhandled_interrupt,    // Bus fault
    unhandled_interrupt,    // Usage fault
    unhandled_interrupt,
    unhandled_interrupt,
    unhandled_interrupt,
    unhandled_interrupt,
    unhandled_interrupt,    // SVC
    unhandled_interrupt,    // Debug monitor
    unhandled_interrupt,
    unhandled_interrupt,    // PendSV
    systick_handler,        // SysTick
};

// vector table for the peripheral interrupts
static VECTOR_TABLE(".irq_vector") void(*IRQ_VECTOR[64])(void) = {
    periph_int_handler, // Power Supply System (PSS) (0)
    periph_int_handler, // Clock System (CS) (1)
    periph_int_handler, // Power Control Manager (PCM) (2)
    periph_int_handler, // Watchdog Timer A (WDT_A) (3)
    fpu_handler,        // FPU_INT, Combined interrupt from flags in FPSCR (4)
    periph_int_handler, // FLash Controller (FLCTL) (5)
    periph_int_handler, // Comparator E0 (6)
    periph_int_handler, // Comparator E1 (7)
    periph_int_handler, // Timer A0 TA0CCTL0.CCIFG (8)
    periph_int_handler, // Timer A0 TA0CCTLx.CCIFG (x = 1 to 4), TA0CTL.TAIFG (9)
    periph_int_handler, // Timer A1 TA1CCTL0.CCIFG (10)
    periph_int_handler, // Timer A1 TA1CCTLx.CCIFG (x = 1 to 4), TA1CTL.TAIFG (11)
    periph_int_handler, // Timer A2 TA2CCTL0.CCIFG (12)
    periph_int_handler, // Timer A2 TA2CCTLx.CCIFG (x = 1 to 4), TA2CTL.TAIFG (13)
    periph_int_handler, // Timer A3 TA3CCTL0.CCIFG (13)
    periph_int_handler, // Timer A3 TA3CCTLx.CCIFG (x = 1 to 4), TA3CTL.TAIFG (15)
    periph_int_handler, // eUSCI A0 (16)
    periph_int_handler, // eUSCI A1 (17)
    periph_int_handler, // eUSCI A2 (18)
    periph_int_handler, // eUSCI A3 (19)
    periph_int_handler, // eUSCI B0 (20)
    periph_int_handler, // eUSCI B1 (21)
    periph_int_handler, // eUSCI B2 (22)
    periph_int_handler, // eUSCI B3 (23)
    periph_int_handler, // Precision ADC (24)
    periph_int_handler, // Timer32 INT1 (25)
    periph_int_handler, // Timer32 INT2 (26)
    periph_int_handler, // Timer32 combined interrupt (27)
    periph_int_handler, // AES256 (28)
    periph_int_handler, // RTC_C (29)
    periph_int_handler, // DMA error (30)
    periph_int_handler, // DMA INT3 (31)
    periph_int_handler, // DMA INT2 (32)
    periph_int_handler, // DMA INT1 (33)
    periph_int_handler, // DMA INT0 (34)
    periph_int_handler, // IO Port 1 (35)
    periph_int_handler, // IO Port 2 (36)
    periph_int_handler, // IO Port 3 (37)
    periph_int_handler, // IO Port 4 (38)
    periph_int_handler, // IO Port 5 (39)
    periph_int_handler, // IO Port 6 (40)
    periph_int_handler, // Reserved (41)
    periph_int_handler, // Reserved (42)
    periph_int_handler, // Reserved (43)
    periph_int_handler, // Reserved (44)
    periph_int_handler, // Reserved (45)
    periph_int_handler, // Reserved (46)
    periph_int_handler, // Reserved (47)
    periph_int_handler, // Reserved (48)
    periph_int_handler, // Reserved (49)
    periph_int_handler, // Reserved (50)
    periph_int_handler, // Reserved (51)
    periph_int_handler, // Reserved (52)
    periph_int_handler, // Reserved (53)
    periph_int_handler, // Reserved (54)
    periph_int_handler, // Reserved (55)
    periph_int_handler, // Reserved (56)
    periph_int_handler, // Reserved (57)
    periph_int_handler, // Reserved (58)
    periph_int_handler, // Reserved (59)
    periph_int_handler, // Reserved (60)
    periph_int_handler, // Reserved (61)
    periph_int_handler, // Reserved (62)
    periph_int_handler, // Reserved (63)
};
