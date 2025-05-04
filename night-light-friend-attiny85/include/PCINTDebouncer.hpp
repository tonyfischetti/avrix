#pragma once

#include <stdint.h>
#include "hal.hpp"

//  TODO  have to already configure as input? (and possibly pull-up?)

template<typename Pin, uint32_t debounce_delay_ms = 50>
class PCINTDebouncer {
    //  TODO  document this
    //  means you can only ever use one instance of PCINTDebouncer<Pin> at a time. If you make two (e.g. for rotary encoder A and B), the second one will overwrite self
    static PCINTDebouncer* self;

    volatile bool interruptedP { false };
    volatile bool rawState {};
    bool stableState {};
    uint32_t lastChangeTime {};

    void (*onFalling)() = nullptr;
    void (*onRising)() = nullptr;

  public:
    PCINTDebouncer(Pin& pin, bool initialState)
      : rawState { initialState },
        stableState { initialState },
        lastChangeTime { 0 } {
        self = this;
    }

    __attribute__((always_inline)) inline static void onISRTriggered(bool state) {
        if (self) {
            self->rawState = state;
            self->interruptedP = true;
        }
    }

    bool hasUpdate() {
        bool wasInterrupted;

        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            wasInterrupted = interruptedP;
            interruptedP = false;
        }

        if (!wasInterrupted) return false;

        interruptedP = false;
        uint32_t now = HAL::Ticker::get_ticks();

        if (rawState != stableState && ((now - lastChangeTime) >= debounce_delay_ms)) {

            if (stableState && !rawState && onFalling) onFalling();
            if (!stableState && rawState && onRising) onRising();

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

    void setOnFalling(void (*callback)()) {
        onFalling = callback;
    }

    void setOnRising(void (*callback)()) {
        onRising = callback;
    }

};

template<typename Pin, uint32_t debounce_delay_ms>
PCINTDebouncer<Pin, debounce_delay_ms>* PCINTDebouncer<Pin, debounce_delay_ms>::self = nullptr;

