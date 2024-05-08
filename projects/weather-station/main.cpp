// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2021
 * E-Mail: hotschi@gmx.at
 */

#include <array>
#include <span>

#include "cs.h"
#include "gpio.h"
#include "i2c.h"
#include "int_pin.h"
#include "led.h"
#include "msp432.h"
#include "pin.h"
#include "uart.h"

constexpr uint16_t BMS_ADDR = 0x76;
constexpr std::array<uint8_t, 1> READ_ID = {0xF3};

std::array<uint8_t, 12> i2c_buf = {};

Msp432& chip = Msp432::instance();
Uart uart0{chip.uscia0(), chip.dma(), 115200, 0, 1, 1, 1};
I2cMaster i2c0{chip.uscib0(), I2cSpeed::KHz100};

static void u8_to_hex(uint8_t val, uint8_t *str)
{
    constexpr char LOOKUP[] = "0123456789ABCDEF";

    str[0] = LOOKUP[(val >> 4) & 0xF];
    str[1] = LOOKUP[val & 0xF];
}

void i2c_cb(I2cJobType t, I2cErr err, std::span<uint8_t> rxbuf, void *cookie) noexcept
{
    Uart *u = reinterpret_cast<Uart*>(cookie);
    uint8_t hex[128];

    switch (err) {
    case I2cErr::Ok:
        for (size_t i = 0; i < rxbuf.size(); i++) {
            u8_to_hex(rxbuf[i], &hex[i * 3]);
            hex[i * 3 + 2] = ' ';
        }
        u->write("I2C callback no error, data: ");
        u->write(std::span<uint8_t>{hex, rxbuf.size() * 3});
        u->write("\r\n");
        break;

    case I2cErr::Nack:
        u->write("I2C callback NACK\r\n");
        break;

    case I2cErr::ArbitrationLost:
        u->write("I2C callback ArbitrationLost\r\n");
        break;

    case I2cErr::ClockLowTimeout:
        u->write("I2C callback ClockLowTimeout\r\n");
        break;
    }
}

int main(void)
{
    chip.init();

    // UART0 pin + driver setup
    chip.gpio_pins().int_pin(IntPinNr::P01_2).enable_primary_function();
    chip.gpio_pins().int_pin(IntPinNr::P01_3).enable_primary_function();
    uart0.init(chip.cs());

    // I2C0 pin + driver setup
    IntPin& sda = chip.gpio_pins().int_pin(IntPinNr::P01_6);
    IntPin& scl = chip.gpio_pins().int_pin(IntPinNr::P01_7);
    sda.make_input();
    scl.make_input();
    sda.enable_primary_function(); // SDA
    scl.enable_primary_function(); // SCL
    i2c0.init(chip.cs());

    Led led_red = Led{chip.gpio_pins().int_pin(IntPinNr::P02_0), false};
    Led led_green = Led{chip.gpio_pins().int_pin(IntPinNr::P02_1), false};
    Led led_blue = Led{chip.gpio_pins().int_pin(IntPinNr::P02_2), false};

    led_red.init();
    led_green.init();
    led_blue.init();

    led_blue.on();
    uart0.write("\r\nHallo erstmal!\r\n");
    i2c0.write_read(BMS_ADDR, std::span{READ_ID}, std::span{i2c_buf}, &uart0, i2c_cb);

    while (true) {
        // uart0.write("loop\r\n");
        led_green.toggle();
        chip.delay_ms(500);
    }
}
