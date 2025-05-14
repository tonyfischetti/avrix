
#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <ostream>

// connection coefficients: 00011101


struct LFSR {
    LFSR(uint8_t seed)
        : state { static_cast<uint8_t>(seed ? seed : 0xAF) } {
    }

    uint8_t next() {
        const uint8_t randBit = state & 0x01;
        // old const uint8_t freshBit = ((((state >> 7) ^ (state >> 5)) ^ (state >> 4)) ^ (state >> 3)) & 0x01;
        const uint8_t freshBit = ((((state >> 4) ^ (state >> 3)) ^ (state >> 2)) ^ (state)) & 0x01;
        // 3, 4, 5, 7 (starting from 0 on the right)
        state = (freshBit << 7) | (state >> 1);
        return randBit;
    }

    uint8_t nextByte() {
        uint8_t randomByte = 0;
        for (uint8_t b = 0; b < 8; b++) {
            randomByte |= ((next() & 1) << (7 - b));
        }
        return randomByte;
    }

    uint8_t state;
};


void printleds(uint8_t currentRand) {

    if ((currentRand & (0x03) << 0)) std::cout << "1";
    else                             std::cout << "0";
    if ((currentRand & (0x03) << 2)) std::cout << "1";
    else                             std::cout << "0";
    if ((currentRand & (0x03) << 4) ||
       !(currentRand & (0x0f)))      std::cout << "1";
    else                             std::cout << "0";
    std::cout << std::endl;
}


int main() {
    
    LFSR lfsr(93);
    // // LFSR lfsr(130);
    // LFSR lfsr(200);
    // LFSR lfsr(0);
    // for (uint32_t i = 0; i < 5000000; i++) {
    for (uint32_t i = 0; i < 10000; i++) {
        uint8_t rb = lfsr.nextByte();
        // uint8_t rb = static_cast<uint8_t>(rand());
        // std::cout << (int)rb << std::endl;
        printleds(rb);
    }


    return 0;
}


/*
 * do we have to "whiten"?
 * LFSR has uneven byte distributions when you bundle 8 bits directly)
 */
