// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2022
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>

enum class Err : uint8_t {
    Ok = 0,
    NotOk,
    Busy,
    OutOfRange,
    NoMem,
    Empty,
    Overflow,
    Underflow,
    NotSupported,
    NotImplemented,
    NotInitialized,
    AlreadyInitialized,
    NullPtr,
};

constexpr const char* err_to_string(const Err& err) noexcept {
    switch (err) {
    case Err::Ok: return "Ok";
    case Err::NotOk: return "NotOk";
    case Err::Busy: return "Busy";
    case Err::OutOfRange: return "OutOfRange";
    case Err::NoMem: return "NoMem";
    case Err::Empty: return "Empty";
    case Err::Overflow: return "Overflow";
    case Err::Underflow: return "Underflow";
    case Err::NotSupported: return "NotSupported";
    case Err::NotImplemented: return "NotImplemented";
    case Err::NotInitialized: return "NotInitialized";
    case Err::AlreadyInitialized: return "AlreadyInitialized";
    case Err::NullPtr: return "NullPtr";
    default: return "No valid errorcode";
    }
}
