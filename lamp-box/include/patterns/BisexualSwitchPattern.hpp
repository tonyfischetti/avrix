#pragma once

#include <stdint.h>
#include "neopixel.hpp"
#include "Pattern.hpp"


template<uint8_t TOTAL_ROWS>
struct BisexualSwitchPattern final : Pattern {

    enum class PatternState : uint8_t { COLOR_1, COLOR_2, COLOR_3 };

    uint16_t tick() override {
        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };
        if (totalPixels == 0) {
            flood_pixels(0, 0, 0, 0, TOTAL_ROWS*8);
            return strobeInterval;
        }
        switch (state) {
            case PatternState::COLOR_1:
                flood_pixels(255, 0, 121, 0, totalPixels);
                state = PatternState::COLOR_2;
                break;
            case PatternState::COLOR_2:
                flood_pixels(255, 0, 255, 0, totalPixels);
                state = PatternState::COLOR_3;
                break;
            case PatternState::COLOR_3:
                flood_pixels(0, 0, 255, 0, totalPixels);
                state = PatternState::COLOR_1;
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
    PatternState state      { PatternState::COLOR_1 };
    uint8_t numRows         { 1 };
    uint8_t brightness      { 5 };
    uint16_t strobeInterval { 250 };

};
