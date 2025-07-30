#pragma once

#include <stdint.h>
#include "neopixel.hpp"

#include "Pattern.hpp"
#include "utils/LFSR.hpp"

struct CandlePattern final : Pattern {

    CandlePattern(uint8_t _numPixels, uint8_t* _pixelArray)
      : numPixels    {  _numPixels },
        pixelArray   { _pixelArray } {
        HAL::Utils::Random::LFSR::init(jumpingSeed);
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
    
    uint16_t tick() override {
        uint8_t currentRand { static_cast<uint8_t>(HAL::Utils::Random::LFSR::nextByte()) };

        for (uint8_t i = 0; i < numPixels; i += 4) {
            currentRand = static_cast<uint8_t>(HAL::Utils::Random::LFSR::nextByte());

            // 0 - 140 - 55%
            if (currentRand < 140) {
                pixelArray[i]   = redChannel;
                pixelArray[i+1] = greenChannel;
                pixelArray[i+2] = blueChannel;
                pixelArray[i+3] = 0;
            }
            // 140 - 210 - 27%
            else if (currentRand < 210) {
                pixelArray[i]   = 30;
                pixelArray[i+1] = 9;
                pixelArray[i+2] = 0;
                pixelArray[i+3] = 0;
            }
            // 210 - 240 - 12%
            else if (currentRand < 240) {
                pixelArray[i]   = 30;
                pixelArray[i+1] = 8;
                pixelArray[i+2] = 0;
                pixelArray[i+3] = 0;
            }
            // 255-240 - 6%
            else {
                pixelArray[i]   = 0;
                pixelArray[i+1] = 0;
                pixelArray[i+2] = 0;
                pixelArray[i+3] = 0;
            }
        }

        if (currentRand % 5 == 0) {
            uint8_t start { 116 };
            // uint8_t start { 112 };
            if      (currentRand > 191) { start =   0; }
            // else if (currentRand > 127) { start =  16; }
            else if (currentRand > 127) { start =  20; }
            else if (currentRand > 63)  { start =  96; }
            for (uint8_t i = start; i < start+12; i+=4) {
            // for (uint8_t i = start; i < start+16; i+=4) {
                pixelArray[i]   = 0;
                pixelArray[i+1] = 0;
                pixelArray[i+2] = 0;
                pixelArray[i+3] = 0;
            }
        }


        sendArray();

        jumpingSeed++;
        if (jumpingSeed == 0) jumpingSeed++;
        HAL::Utils::Random::LFSR::init(jumpingSeed);

        if (currentRand < 50)
            return interval*5;
        return interval;
    }

    void onCW() override {
        interval += 2;
    }

    void onCCW() override {
        interval -= 2;
    }

  private:
    uint8_t  jumpingSeed        {   93 };
    uint16_t interval           {   50 };
    const uint8_t  redChannel   {   30 };
    const uint8_t  greenChannel {   10 };
    const uint8_t  blueChannel  {    1 };
    const uint8_t  numPixels    {      };
          uint8_t* pixelArray   {      };

};


