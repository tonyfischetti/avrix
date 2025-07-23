#pragma once

#include <stdint.h>
#include "neopixel.hpp"

#include "Pattern.hpp"
#include "utils/LFSR.hpp"


template<uint8_t TOTAL_ROWS>
struct CandlePattern final : Pattern {

    CandlePattern() {
        HAL::Utils::Random::LFSR::init(93);
    }
    
    inline void on() {
        send_pixel(redChannel, greenChannel, blueChannel, 0);
    }
    inline void off() {
        send_pixel(0, 0, 0, 0);
    }

    uint16_t tick() override {
        uint8_t currentRand { static_cast<uint8_t>(HAL::Utils::Random::LFSR::nextByte()) };

        flood_pixels(0, 0, 0, 0, 10);

        if ((currentRand & (0x03) << 0))  on();
        else                             off();
        send_pixel(redChannel, greenChannel, blueChannel, 0);
        send_pixel(redChannel, greenChannel, blueChannel, 0);
        if ((currentRand & (0x03) << 4) ||
           !(currentRand & (0x0f)))       on();
        else                             off();
        flood_pixels(0, 0, 0, 0, 4);


        send_pixel(redChannel, greenChannel, blueChannel, 0);

        if ((currentRand & (0x03) << 2))  on();
        else                             off();

        send_pixel(redChannel, greenChannel, blueChannel, 0);
        send_pixel(redChannel, greenChannel, blueChannel, 0);

        return interval;
    }

    void onCW() override {
        interval += 2;
    }

    void onCCW() override {
        interval -= 2;
    }

    void onRelease() override {
        numRows = (numRows + 1) % TOTAL_ROWS;
    }

  private:
    uint8_t numRows            {    1 };
    uint16_t interval          {  200 };
    const uint8_t redChannel   {   30 };
    const uint8_t greenChannel {   10 };
    const uint8_t blueChannel  {    1 };

};
