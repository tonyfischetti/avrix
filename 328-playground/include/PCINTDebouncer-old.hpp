#pragma once

#include <stdint.h>
#include <util/atomic.h>
#include "../avr-hal/include/hal.hpp"


//  TODO  have to already configure as input? (and possibly pull-up?)

template<typename Pin, uint32_t debounceDelayMS = 50>
class PCINTDebouncer {

    volatile bool interruptedP { false };
    volatile bool rawState {};
    bool stableState {};
    uint32_t lastChangeTime {};

    // void (*onFalling)() = nullptr;
    // void (*onRising)() = nullptr;

  public:
    PCINTDebouncer(Pin& pin, bool initialState)
      : rawState { initialState },
        stableState { initialState },
        lastChangeTime { 0 } {
    }

    bool hasUpdate() {
        bool wasInterrupted;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            wasInterrupted = interruptedP;
            interruptedP = false;
        }

        if (!wasInterrupted) return false;

        interruptedP = false;
        uint32_t now = HAL::Ticker::getNumTicks();

        if (rawState != stableState && ((now - lastChangeTime) >= debounceDelayMS)) {

            // if (stableState && !rawState && onFalling) onFalling();
            // if (!stableState && rawState && onRising) onRising();

            stableState = rawState;
            lastChangeTime = now;
            return true;
        }
        // bouncing or no change
        return false;
    }

    bool state() const {
        return stableState;
    }

    // void setOnFalling(void (*callback)()) {
    //     onFalling = callback;
    // }
    //
    // void setOnRising(void (*callback)()) {
    //     onRising = callback;
    // }

};


