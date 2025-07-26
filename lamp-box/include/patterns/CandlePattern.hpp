#pragma once

#include <stdint.h>
#include "neopixel.hpp"

#include "Pattern.hpp"
#include "utils/LFSR.hpp"

struct CandlePattern final : Pattern {

    CandlePattern(uint8_t _numPixels, uint8_t* _pixelArray)
      : numPixels    {  _numPixels },
        pixelArray   { _pixelArray } {
        HAL::Utils::Random::LFSR::init(93);
    }

    //  TODO  implement this is assembly
    //        include a LIMIT and OFFSET
    inline void sendArray() {
        for (uint8_t i = 0; i < numPixels; i += 4) {
            send_pixel(pixelArray[i],
                       pixelArray[i+1],
                       pixelArray[i+2],
                       pixelArray[i+3]);
        }
    }
    
    inline void on() {
        send_pixel(redChannel, greenChannel, blueChannel, 0);
    }
    inline void off() {
        send_pixel(0, 0, 0, 0);
    }

    uint16_t tick() override {
        uint8_t currentRand { static_cast<uint8_t>(HAL::Utils::Random::LFSR::nextByte()) };

        if (currentRand < 100) {
            for (uint8_t i = 0; i < numPixels; i += 4) {
                currentRand = static_cast<uint8_t>(HAL::Utils::Random::LFSR::nextByte());

                if (currentRand < 140) {
                    pixelArray[i]   = redChannel;
                    pixelArray[i+1] = greenChannel;
                    pixelArray[i+2] = blueChannel;
                    pixelArray[i+3] = 0;
                }
                else if (currentRand < 220) {
                    pixelArray[i]   = 30;
                    pixelArray[i+1] = 8;
                    pixelArray[i+2] = 0;
                    pixelArray[i+3] = 0;
                }
                else if (currentRand < 240) {
                    pixelArray[i]   = 30;
                    pixelArray[i+1] = 6;
                    pixelArray[i+2] = 0;
                    pixelArray[i+3] = 0;
                }
                else {
                    pixelArray[i]   = 0;
                    pixelArray[i+1] = 0;
                    pixelArray[i+2] = 0;
                    pixelArray[i+3] = 0;
                }
            }
        }

        sendArray();

        return interval;
    }

    void onCW() override {
        interval += 2;
    }

    void onCCW() override {
        interval -= 2;
    }

  private:
    uint16_t interval           {   50 };
    const uint8_t  redChannel   {   30 };
    const uint8_t  greenChannel {   10 };
    const uint8_t  blueChannel  {    1 };
    const uint8_t  numPixels    {      };
          uint8_t* pixelArray   {      };

};


