// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#include "cm4f.h"
#include "fpu.h"
#include "fpu_regs.h"
#include "helpers.h"

constexpr uint32_t EX_FLUSH_TO_ZERO = hlp::bit<uint32_t>(7);
constexpr uint32_t EX_INEXACT_RESULT = hlp::bit<uint32_t>(4);
constexpr uint32_t EX_UNDERFLOW = hlp::bit<uint32_t>(3);
constexpr uint32_t EX_OVERFLOW = hlp::bit<uint32_t>(2);
constexpr uint32_t EX_DIVISION_BY_ZERO = hlp::bit<uint32_t>(1);
constexpr uint32_t EX_INVALID_OPERATION = hlp::bit<uint32_t>(0);

void Fpu::enable() noexcept
{
    // enable the FPU co-processors
    reg().cpacr.set(fpuregs::cpacr::cp10.value(3) + fpuregs::cpacr::cp11.value(3));

    // Disable automatic preserving of the FPSCR register -> enables direct reading of this register
    // instead of using the dumped version on the stack.
    reg().fpccr.modify(fpuregs::fpccr::aspen.value(0));

    cm4f::set_fpscr(0);
}

void Fpu::disable() noexcept
{
    reg().cpacr.set(0);
}

void Fpu::handle_interrupt() noexcept
{
    constexpr uint32_t EXCEPTIONS =
        EX_INEXACT_RESULT
        | EX_UNDERFLOW
        | EX_OVERFLOW
        | EX_DIVISION_BY_ZERO
        | EX_INVALID_OPERATION;

    // FIXME: The only thing we currently do here is to delete all exception flags and simply
    // continue. Not sure if this is really what we want though..   
    uint32_t fpscr = cm4f::get_fpscr();
    cm4f::set_fpscr(fpscr & (~EXCEPTIONS));
}

void Fpu::set_rounding_mode(RoundingMode mode) noexcept
{
    reg().fpdscr.modify(fpuregs::fpdscr::rmode.value(static_cast<uint32_t>(mode)));
}
