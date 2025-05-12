#pragma once

#include "hal_common.hpp"

#include <stdint.h>

/**
 * implementation of an 8-bit fibonacci linear feedback shift register
 * uses connection coefficient of 01010101
 */

namespace HAL {
namespace Utils {
namespace Random {

struct LFSR {

    LFSR(uint8_t seed)
        : state { static_cast<uint8_t>(seed ? seed : 0xAF) } { }

    uint8_t nextBit() {
        const uint8_t randBit = state & 0x01;
        const uint8_t freshBit = ((((state >> 7) ^ (state >> 5)) ^ (state >> 4)) ^ (state >> 3)) & 0x01;
        state = (freshBit << 7) | (state >> 1);
        return randBit;
    }

    uint8_t nextByte() {
        uint8_t randomByte = 0;
        for (uint8_t b = 0; b < 8; b++) {
            randomByte |= ((nextBit() & 1) << (7 - b));
        }
        return randomByte;
    }

  private:
    uint8_t state;

};

}
}
}
