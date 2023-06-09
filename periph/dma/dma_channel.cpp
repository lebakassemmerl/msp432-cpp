// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include <bit>

#include "err.h"
#include "helpers.h"
#include "register.h"

#include "dma_regs.h"
#include "dma.h"


Err DmaChannel::setup(const DmaConfig& conf) noexcept
{
    if (in_use)
        return Err::AlreadyInitialized;

    if (!conf.done || !conf.instance)
        return Err::NullPtr;

    this->conf = conf;
    ctrl_prim.ctrl.modify(
        dmactrl::ctrl::src_size.value(static_cast<uint32_t>(conf.width))
        + dmactrl::ctrl::dst_size.value(static_cast<uint32_t>(conf.width))
        + dmactrl::ctrl::src_inc.value(static_cast<uint32_t>(conf.src_incr))
        + dmactrl::ctrl::dst_inc.value(static_cast<uint32_t>(conf.dst_incr))
    );

    reg().chx_srccfg[idx].set(conf.src_chan % (MAX_SRC_NR + 1));

    in_use = true;
    return Err::Ok;
}

Err DmaChannel::transfer_mem_to_periph(const uint8_t* src, uint8_t* dst, uint32_t len) noexcept
{
    if (!in_use)
        return Err::NotInitialized;

    if ((!src) || (!dst))
        return Err::NullPtr;

    if (len == 0)
        return Err::Empty;

    if (info.busy)
        return Err::Busy;

    info.busy = true;

    uint32_t src_end_ptr = reinterpret_cast<uint32_t>(src) + (len - 1);
    uint32_t dst_end_ptr = reinterpret_cast<uint32_t>(dst);

    mode = DmaMode::Basic;
    ctrl_prim.src_ptr.set(src_end_ptr);
    ctrl_prim.dst_ptr.set(dst_end_ptr);
    ctrl_prim.ctrl.modify(
        dmactrl::ctrl::src_inc.value(static_cast<uint32_t>(conf.src_incr))
        + dmactrl::ctrl::dst_inc.value(static_cast<uint32_t>(conf.dst_incr))
    );

    info.src = src;
    info.dst = dst;
    info.num_bytes = len;

    calc_remaining_words(len);
    info.type = DmaTransferType::MemoryToPeripheral;

    config_prim_channel(len);
    enable_channel();

    return Err::Ok;
}

Err DmaChannel::transfer_periph_to_mem(const uint8_t* src, uint8_t* dst, uint32_t len) noexcept
{
    if (!in_use)
        return Err::NotInitialized;

    if ((!src) || (!dst))
        return Err::NullPtr;

    if (len == 0)
        return Err::Empty;

    if (info.busy)
        return Err::Busy;

    info.busy = true;

    uint32_t src_end_ptr = reinterpret_cast<uint32_t>(src);
    uint32_t dst_end_ptr = reinterpret_cast<uint32_t>(dst) + (len - 1);

    mode = DmaMode::Basic;
    ctrl_prim.src_ptr.set(src_end_ptr);
    ctrl_prim.dst_ptr.set(dst_end_ptr);
    ctrl_prim.ctrl.modify(
        dmactrl::ctrl::src_inc.value(static_cast<uint32_t>(conf.src_incr))
        + dmactrl::ctrl::dst_inc.value(static_cast<uint32_t>(conf.dst_incr))
    );

    info.src = src;
    info.dst = dst;
    info.num_bytes = len;

    calc_remaining_words(len);
    info.type = DmaTransferType::PeripheralToMemory;

    config_prim_channel(len);
    enable_channel();

    return Err::Ok;
}

Err DmaChannel::transfer_custom(const uint8_t* src, uint8_t* dst, DmaPtrIncrement src_incr,
    DmaPtrIncrement dst_incr, uint32_t num_bytes) noexcept
{
    if (!in_use)
        return Err::NotInitialized;

    if ((!src) || (!dst))
        return Err::NullPtr;

    if (num_bytes == 0)
        return Err::Empty;

    if (info.busy)
        return Err::Busy;

    info.busy = true;

    uint32_t src_end_ptr = reinterpret_cast<uint32_t>(src);
    uint32_t dst_end_ptr = reinterpret_cast<uint32_t>(dst);
    if (src_incr != DmaPtrIncrement::NoIncr)
        src_end_ptr += num_bytes - 1;

    if (dst_incr != DmaPtrIncrement::NoIncr)
        dst_end_ptr += num_bytes - 1;

    mode = DmaMode::Basic;
    ctrl_prim.src_ptr.set(src_end_ptr);
    ctrl_prim.dst_ptr.set(dst_end_ptr);
    ctrl_prim.ctrl.modify(
        dmactrl::ctrl::src_inc.value(static_cast<uint32_t>(src_incr))
        + dmactrl::ctrl::dst_inc.value(static_cast<uint32_t>(dst_incr))
    );

    info.src = src;
    info.dst = dst;
    info.num_bytes = num_bytes;

    calc_remaining_words(num_bytes);
    info.type = DmaTransferType::PeripheralToMemory;

    config_prim_channel(num_bytes);
    enable_channel();

    return Err::Ok;
}

void DmaChannel::calc_remaining_words(size_t num_bytes) noexcept
{
    size_t transfers = num_bytes >> static_cast<size_t>(conf.width);
    if (transfers > MAX_TRANSFERS_LEN)
        info.remaining_words = transfers - MAX_TRANSFERS_LEN;
    else
        info.remaining_words = 0;
}

void DmaChannel::update_dma_pointers() noexcept
{
    uint32_t r_power = 0;

    if ((info.type == DmaTransferType::PeripheralToMemory)
        || (info.type == DmaTransferType::MemoryToMemory)) {
        ctrl_prim.dst_ptr.set(
            ctrl_prim.dst_ptr.get() + (MAX_TRANSFERS_LEN << static_cast<uint32_t>(conf.width)));
    }

    if ((info.type == DmaTransferType::MemoryToPeripheral)
        || (info.type == DmaTransferType::MemoryToMemory)) {
        ctrl_prim.src_ptr.set(
            ctrl_prim.src_ptr.get() + (MAX_TRANSFERS_LEN << static_cast<uint32_t>(conf.width)));
    }

    if (info.remaining_words > MAX_TRANSFERS_LEN)
        info.remaining_words -= MAX_TRANSFERS_LEN;
    else
        info.remaining_words = 0;

    if (info.type == DmaTransferType::MemoryToMemory) {
        if (info.remaining_words > MAX_TRANSFERS_LEN)
            r_power = 31 - std::countl_zero(MAX_TRANSFERS_LEN);
        else
            r_power = 31 - std::countl_zero(info.remaining_words);
    }

    ctrl_prim.ctrl.modify(
        dmactrl::ctrl::cycle_ctrl.value(static_cast<uint32_t>(mode))
        + dmactrl::ctrl::n_minus_1.value((info.remaining_words - 1) % MAX_TRANSFERS_LEN)
        + dmactrl::ctrl::r_power.value(r_power)
    );
}

void DmaChannel::config_prim_channel(uint32_t bytes_to_transmit) noexcept
{
    uint32_t transfers = bytes_to_transmit >> static_cast<uint32_t>(conf.width);

    ctrl_prim.ctrl.modify(
        dmactrl::ctrl::n_minus_1.value((transfers - 1) % MAX_TRANSFERS_LEN)
        + dmactrl::ctrl::r_power.value(0)
        + dmactrl::ctrl::cycle_ctrl.value(static_cast<uint32_t>(mode))
    );
}

void DmaChannel::handle_interrupt() noexcept
{
    if (info.remaining_words > 0) {
        update_dma_pointers();
    } else {
        info.busy = false;
        conf.done(info.src, info.dst, info.num_bytes, conf.instance);
    }
}

void DmaChannel::enable_channel() noexcept
{
    reg().enaset.set(reg().enaset.get() | (1UL << static_cast<uint32_t>(idx)));
}
