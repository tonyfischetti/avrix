#pragma once

#include "hal_common.hpp"

#include <stdint.h>
#include "gpio.hpp"
#include "ticker.hpp"


namespace HAL {

enum class Transition : uint8_t { RISING, FALLING, NONE };

namespace Utils {


template<uint8_t physicalPin>
class TransitionDebouncer {

    HAL::GPIO::GPIO<physicalPin> gpio;
    //  TODO  parameterize
    const uint32_t debounceWaitTime;
    bool stableState;

  public:
    volatile uint32_t lastUnprocessedInterrupt;

    TransitionDebouncer(bool _initial, uint32_t _debounceWaitTime, bool _pullup=false)
        : debounceWaitTime { _debounceWaitTime },
          stableState { _initial },
          lastUnprocessedInterrupt { 0 } {
        if (_pullup)
            HAL::GPIO::GPIO<physicalPin>::setInputPullup();
        else
            HAL::GPIO::GPIO<physicalPin>::setInput();
        HAL::GPIO::GPIO<physicalPin>::enablePCINT();
    }

    void notifyInterruptOccurred(uint32_t now) {
        if (!lastUnprocessedInterrupt) {
            lastUnprocessedInterrupt = now;
        }
    }

    Transition processAnyInterrupts() {
        Transition transition { Transition::NONE };
        if (lastUnprocessedInterrupt > 0) {
            uint32_t now = HAL::Ticker::getNumTicks();
            if ((static_cast<uint32_t>(
                 (now - lastUnprocessedInterrupt))) >= debounceWaitTime) {
                bool nowState = gpio.read();

                if (nowState && !stableState)
                    transition = Transition::RISING;
                else if (!nowState && stableState)
                    transition = Transition::FALLING;

                stableState = nowState;

                lastUnprocessedInterrupt = 0;
            }
        }
        return transition;
    }
};



}
}

