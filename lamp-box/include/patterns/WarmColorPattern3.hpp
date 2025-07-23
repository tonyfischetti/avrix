#pragma once

#include <stdint.h>
#include "neopixel.hpp"
#include "Pattern.hpp"

template<uint8_t TOTAL_ROWS>
struct WarmColorPattern3 final : Pattern {

    uint16_t tick() override {
        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };
        uint8_t greenChannel = (brightness >> 2) +
                               (brightness >> 3);
        uint8_t blueChannel  = (brightness >> 5);
        if (totalPixels == 0)
            flood_pixels(0, 0, 0, 0, TOTAL_ROWS*8);
        else
            flood_pixels(brightness, greenChannel, blueChannel, 0, totalPixels);
        return 0;
    }

    void onCW() override {
        if (brightness > 245)
            brightness = 255;
        else
            brightness += 10;
    }

    void onCCW() override {
        if (brightness < 10)
            brightness = 0;
        else
            brightness -= 10;
    }

    void onRelease() override {
        numRows = (numRows + 1) % TOTAL_ROWS;
    }

  private:
    uint8_t numRows    { 1 };
    uint8_t brightness { 20 };

};
