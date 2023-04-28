// SPDX-License-Identifier: MIT

/*
 * Created by lebakassemmerl 2023
 * E-Mail: hotschi@gmx.at
 */

#pragma once

#include <cstdint>

#include "register.h"

namespace uscispiregs {
    namespace ctlw0 {
        constexpr BitField<uint16_t> swrst{0, 0};
        constexpr BitField<uint16_t> stem{1, 1};
        constexpr BitField<uint16_t> ssel{7, 6};
        constexpr BitField<uint16_t> sync{8, 8};
        constexpr BitField<uint16_t> mode{10, 9};
        constexpr BitField<uint16_t> mst{11, 11};
        constexpr BitField<uint16_t> sevenbit{12, 12};
        constexpr BitField<uint16_t> msb{13, 13};
        constexpr BitField<uint16_t> ckpl{14, 14};
        constexpr BitField<uint16_t> ckph{15, 15};
    }
    namespace statw {
        constexpr BitField<uint16_t> busy{0, 0};
        constexpr BitField<uint16_t> oe{5, 5};
        constexpr BitField<uint16_t> fe{6, 6};
        constexpr BitField<uint16_t> listen{7, 7};
    }
    namespace ifg {
        constexpr BitField<uint16_t> rx{0, 0};
        constexpr BitField<uint16_t> tx{1, 1};
    }
}
