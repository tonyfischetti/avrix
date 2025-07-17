
#pragma once

#include <stdint.h>
#include "neopixel.hpp"
#include "Pattern.hpp"


template<uint8_t TOTAL_ROWS>
struct WarmStrobePattern final : Pattern {

    enum class PatternState : uint8_t { ON, OFF };

    uint16_t tick() override {
        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };
        if (totalPixels == 0) {
            flood_pixels(0, 0, 0, 0, TOTAL_ROWS*8);
            return strobeInterval;
        }
        switch (state) {
            case PatternState::ON:
                flood_pixels(0, 0, 0, 255, totalPixels);
                state = PatternState::OFF;
                break;
            case PatternState::OFF:
                flood_pixels(0, 0, 0, 0, totalPixels);
                state = PatternState::ON;
                break;
        }
        return strobeInterval;
    }

    void onCW() override {
        strobeInterval += 2;
    }

    void onCCW() override {
        strobeInterval -= 2;
    }

    void onRelease() override {
        numRows = (numRows + 1) % TOTAL_ROWS;
    }


  private:
    PatternState state      { PatternState::ON };
    uint8_t numRows         { 1 };
    uint8_t brightness      { 5 };
    uint16_t strobeInterval { 250 };

};
