#pragma once

#include <stdint.h>
#include "neopixel.hpp"
#include "Pattern.hpp"


template<uint8_t TOTAL_ROWS>
struct ChooseAColorPattern final : Pattern {

    enum class PatternState : uint8_t {
        CROSSFADE_RED_TO_GREEN,
        CROSSFADE_GREEN_TO_BLUE,
        CROSSFADE_BLUE_TO_RED
    };

    uint16_t tick() override {
        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };
        if (totalPixels == 0)
            flood_pixels(0, 0, 0, 0, TOTAL_ROWS*8);
        else
            flood_pixels(redChannel, greenChannel, blueChannel, 0, totalPixels);
        return 0;
    }

    void onCW() override {
        switch (state) {
            case PatternState::CROSSFADE_RED_TO_GREEN:
                if (redChannel < stepDelta)
                    state = PatternState::CROSSFADE_GREEN_TO_BLUE;
                else {
                    redChannel   -= stepDelta;
                    greenChannel += stepDelta;
                }
                break;
            case PatternState::CROSSFADE_GREEN_TO_BLUE:
                if (greenChannel < stepDelta)
                    state = PatternState::CROSSFADE_BLUE_TO_RED;
                else {
                    greenChannel -= stepDelta;
                    blueChannel  += stepDelta;
                }
                break;
            case PatternState::CROSSFADE_BLUE_TO_RED:
                if (blueChannel < stepDelta)
                    state = PatternState::CROSSFADE_RED_TO_GREEN;
                else {
                    blueChannel -= stepDelta;
                    redChannel  += stepDelta;
                }
                break;
        }
    }

    void onCCW() override {
        switch (state) {
            case PatternState::CROSSFADE_RED_TO_GREEN:
                if (greenChannel < stepDelta)
                    state = PatternState::CROSSFADE_GREEN_TO_BLUE;
                else {
                    redChannel   += stepDelta;
                    greenChannel -= stepDelta;
                }
                break;
            case PatternState::CROSSFADE_GREEN_TO_BLUE:
                if (blueChannel < stepDelta)
                    state = PatternState::CROSSFADE_BLUE_TO_RED;
                else {
                    greenChannel += stepDelta;
                    blueChannel  -= stepDelta;
                }
                break;
            case PatternState::CROSSFADE_BLUE_TO_RED:
                if (redChannel < stepDelta)
                    state = PatternState::CROSSFADE_RED_TO_GREEN;
                else {
                    blueChannel += stepDelta;
                    redChannel  -= stepDelta;
                }
                break;
        }
    }

    void onRelease() override {
        numRows = (numRows + 1) % TOTAL_ROWS;
    }


  private:
    PatternState state      { PatternState::CROSSFADE_RED_TO_GREEN };
    uint8_t brightness      { 255 };
    uint8_t redChannel      { brightness };
    uint8_t greenChannel    { 0 };
    uint8_t blueChannel     { 0 };
    uint8_t numRows         { 1 };
    uint8_t stepDelta       { 2 };

};
