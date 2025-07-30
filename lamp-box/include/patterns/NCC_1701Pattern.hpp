#pragma once

#include <stdint.h>
#include "neopixel.hpp"

#include "Pattern.hpp"

struct NCC_1701Pattern final : Pattern {

    enum class PatternState : uint8_t { p00011000,
                                        p00100100,
                                        p01000010,
                                        p10000001 };

    NCC_1701Pattern(uint8_t* _pixelArray)
      : pixelArray   { _pixelArray } {
    }

    //  TODO  implement this is assembly
    //        include a LIMIT and OFFSET
    inline void sendArray() {
        for (uint8_t i = 0; i < 4*8; i += 4) {
            send_pixel(pixelArray[i],
                       pixelArray[i+1],
                       pixelArray[i+2],
                       pixelArray[i+3]);
        }
    }

    void clearRow() {
        for (uint8_t i = 0; i < 4*8; i++) {
            pixelArray[i] = 0;
        }
    }
    
    uint16_t tick() override {
        uint8_t greenChannel = (brightness >> 2) +
                               (brightness >> 4) +
                               (brightness >> 6);
        uint8_t blueChannel  = (brightness >> 5);

        switch (state) {
            case PatternState::p00011000:
                clearRow();
                pixelArray[12] = brightness;
                pixelArray[13] = greenChannel;
                pixelArray[14] = blueChannel;
                pixelArray[15] = 0;
                pixelArray[16] = brightness;
                pixelArray[17] = greenChannel;
                pixelArray[18] = blueChannel;
                pixelArray[19] = 0;
                state = PatternState::p00100100;
                break;
            case PatternState::p00100100:
                clearRow();
                pixelArray[8]  = brightness;
                pixelArray[9]  = greenChannel;
                pixelArray[10] = blueChannel;
                pixelArray[11] = 0;
                pixelArray[20] = brightness;
                pixelArray[21] = greenChannel;
                pixelArray[22] = blueChannel;
                pixelArray[23] = 0;
                state = PatternState::p01000010;
                break;
            case PatternState::p01000010:
                clearRow();
                pixelArray[4]  = brightness;
                pixelArray[5]  = greenChannel;
                pixelArray[6]  = blueChannel;
                pixelArray[7]  = 0;
                pixelArray[24] = brightness;
                pixelArray[25] = greenChannel;
                pixelArray[26] = blueChannel;
                pixelArray[27] = 0;
                state = PatternState::p10000001;
                break;
            case PatternState::p10000001:
                clearRow();
                pixelArray[0]  = brightness;
                pixelArray[1]  = greenChannel;
                pixelArray[2]  = blueChannel;
                pixelArray[3]  = 0;
                pixelArray[28] = brightness;
                pixelArray[29] = greenChannel;
                pixelArray[30] = blueChannel;
                pixelArray[31] = 0;
                state = PatternState::p00011000;
                break;
        }

        sendArray();

        return interval;
    }

    void onCW() override {
        if (brightness > 250)
            brightness = 255;
        else
            brightness += 5;
    }

    void onCCW() override {
        if (brightness < 5)
            brightness = 0;
        else
            brightness -= 5;
    }

  private:
    PatternState state          { PatternState::p00011000 };
    uint8_t brightness          {   25 };
    uint16_t interval           {  600 };
    const uint8_t  redChannel   {   30 };
    const uint8_t  greenChannel {   10 };
    const uint8_t  blueChannel  {    1 };
    const uint8_t  numPixels    {      };
          uint8_t* pixelArray   {      };

};


