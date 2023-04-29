// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <span>

#include "cs.h"
#include "dma.h"
#include "err.h"
#include "fifo.h"
#include "uart.h"
#include "usci.h"
#include "uscia_regs.h"

// the fractional part is a uint16_t scaled with 2^16 to avoid floating point arithmetic
struct BaudFraction {
    static constexpr uint64_t SHIFT = 16; // equals a factor of 65536
    uint16_t frac;
    uint8_t reg_val;
} __attribute__((packed));

constexpr BaudFraction BAUD_FRACTIONS[36] = {
    BaudFraction { .frac =     0, .reg_val = 0x00 },
    BaudFraction { .frac =  3466, .reg_val = 0x01 },
    BaudFraction { .frac =  4685, .reg_val = 0x02 },
    BaudFraction { .frac =  5472, .reg_val = 0x04 },
    BaudFraction { .frac =  6560, .reg_val = 0x08 },
    BaudFraction { .frac =  8205, .reg_val = 0x10 },
    BaudFraction { .frac =  9371, .reg_val = 0x20 },
    BaudFraction { .frac = 10944, .reg_val = 0x11 },
    BaudFraction { .frac = 14070, .reg_val = 0x21 },
    BaudFraction { .frac = 14575, .reg_val = 0x22 },
    BaudFraction { .frac = 16403, .reg_val = 0x44 },
    BaudFraction { .frac = 19660, .reg_val = 0x25 },
    BaudFraction { .frac = 21856, .reg_val = 0x49 },
    BaudFraction { .frac = 23429, .reg_val = 0x4A },
    BaudFraction { .frac = 24595, .reg_val = 0x52 },
    BaudFraction { .frac = 26234, .reg_val = 0x92 },
    BaudFraction { .frac = 28088, .reg_val = 0x53 },
    BaudFraction { .frac = 28691, .reg_val = 0x55 },
    BaudFraction { .frac = 32781, .reg_val = 0xAA },
    BaudFraction { .frac = 37453, .reg_val = 0x6B },
    BaudFraction { .frac = 39341, .reg_val = 0xAD },
    BaudFraction { .frac = 40986, .reg_val = 0xB5 },
    BaudFraction { .frac = 42152, .reg_val = 0xB6 },
    BaudFraction { .frac = 43692, .reg_val = 0xD6 },
    BaudFraction { .frac = 45881, .reg_val = 0xB7 },
    BaudFraction { .frac = 46838, .reg_val = 0xBB },
    BaudFraction { .frac = 49171, .reg_val = 0xDD },
    BaudFraction { .frac = 51517, .reg_val = 0xED },
    BaudFraction { .frac = 52455, .reg_val = 0xEE },
    BaudFraction { .frac = 54611, .reg_val = 0xBF },
    BaudFraction { .frac = 55469, .reg_val = 0xDF },
    BaudFraction { .frac = 56177, .reg_val = 0xEF },
    BaudFraction { .frac = 57350, .reg_val = 0xF7 },
    BaudFraction { .frac = 59008, .reg_val = 0xFB },
    BaudFraction { .frac = 60096, .reg_val = 0xFD },
    BaudFraction { .frac = 60869, .reg_val = 0xFE },
};

err::Err Uart::init(const Cs& cs) noexcept
{
    err::Err ret;

    auto& regs = usci.reg();

    // disable the peripheral before setting it up
    regs.ctlw0.set(usciaregs::ctlw0::swrst.value(1));

    regs.ctlw0.modify(
        usciaregs::ctlw0::mode.value(3)         // enable UART mode
        + usciaregs::ctlw0::sync.value(0)       // enable asynchronous mode
        + usciaregs::ctlw0::ssel.value(3)       // use SMCLK as clock-source for the UART
        + usciaregs::ctlw0::sevenbit.value(0)   // disable 7-bit mode
        + usciaregs::ctlw0::spb.value(0)        // use 1 stop-bit
        + usciaregs::ctlw0::pen.value(0));      // disable parity bits

    // the following calculation is done using the formulars in the datasheet on page 915
    uint64_t n_scaled = static_cast<uint64_t>(cs.sm_clk() / baud) << BaudFraction::SHIFT;
    uint64_t n_float = (static_cast<uint64_t>(cs.sm_clk()) << BaudFraction::SHIFT)
        / static_cast<uint64_t>(baud);
    uint16_t frac_part = static_cast<uint16_t>(n_float - n_scaled);
    uint16_t n = static_cast<uint16_t>(n_scaled >> BaudFraction::SHIFT);
    if (n > 16) {
        // oversampling is enabled
        regs.brw.set(n >> 4);
        uint16_t brf = static_cast<uint16_t>(
            ((n_float >> 4) - (static_cast<uint64_t>(n >> 4) << BaudFraction::SHIFT))
            >> (BaudFraction::SHIFT - 4)
        );
        regs.mctlw.modify(usciaregs::mctlw::brf.value(brf) + usciaregs::mctlw::os16.value(1));
    } else {
        regs.brw.set(n);
        regs.mctlw.modify(usciaregs::mctlw::os16.value(0));
    }

    // look for the closest calibration value
    // according to the datasheet not the closest value should be taken but the next smaller one
    uint8_t cal_val = BAUD_FRACTIONS[0].reg_val;
    for (auto& val : BAUD_FRACTIONS) {
        if (val.frac > frac_part)
            break;

        cal_val = val.reg_val;
    }
    regs.mctlw.modify(usciaregs::mctlw::brs.value(cal_val));
    regs.ctlw0.modify(usciaregs::ctlw0::swrst.value(0)); // enable the module

    DmaConfig tx_cfg{};
    tx_cfg.src_chan = tx_dma_src;
    tx_cfg.width = DmaDataWidth::Width8Bit;
    tx_cfg.src_incr = DmaPtrIncrement::Incr8Bit;
    tx_cfg.dst_incr = DmaPtrIncrement::NoIncr;
    tx_cfg.instance = reinterpret_cast<void*>(this);
    tx_cfg.done = Uart::redirect_tx_handler;

    DmaConfig rx_cfg{};
    rx_cfg.src_chan = rx_dma_src;
    rx_cfg.width = DmaDataWidth::Width8Bit;
    rx_cfg.src_incr = DmaPtrIncrement::NoIncr;
    rx_cfg.dst_incr = DmaPtrIncrement::Incr8Bit;
    rx_cfg.instance = reinterpret_cast<void*>(this);
    rx_cfg.done = Uart::redirect_rx_handler;

    ret = tx_dma.setup(tx_cfg);
    if (ret != err::Err::Ok)
        return ret;

    ret = rx_dma.setup(rx_cfg);
    if (ret != err::Err::Ok)
        return ret;

    initialized = true;
    return err::Err::Ok;
}

err::Err Uart::write(std::span<uint8_t> data) noexcept
{
    if (!initialized)
        return err::Err::NotInitialized;

    if (tx_jobs.is_full())
        return err::Err::NoMem;

    tx_jobs.push(data);

    if (!tx_dma.transfer_going())
        queue_tx_job();

    return err::Err::Ok;
}

void Uart::queue_tx_job() noexcept
{
    if (tx_jobs.is_empty())
        return;

    auto& job = tx_jobs.peek().second.value().get();
    tx_dma.transfer_mem_to_periph(
        job.data(), reinterpret_cast<uint8_t*>(&usci.reg().txbuf), job.size());
}

void Uart::redirect_tx_handler(
    const uint8_t* src_buf, uint8_t* dst_buf, size_t len, void* instance) noexcept
{
    Uart* inst = reinterpret_cast<Uart*>(instance);
    inst->tx_handler(src_buf, len);
}

void Uart::redirect_rx_handler(
    const uint8_t* src_buf, uint8_t* dst_buf, size_t len, void* instance) noexcept
{
    Uart* inst = reinterpret_cast<Uart*>(instance);
    inst->rx_handler(dst_buf, len);
}

void Uart::tx_handler(const uint8_t* buf, size_t len) noexcept
{
    tx_jobs.pop();
    queue_tx_job();
}

void Uart::rx_handler(uint8_t* buf, size_t len) noexcept
{

}
