#include "avr-hal.hpp"

class PCINTDebouncer {

    bool stableState;
    uint32_t lastChangeTime;
    uint32_t lockOutPeriod;

  public:

    PCINTDebouncer(bool _initialState, uint32_t _lockOutPeriod)
      : stableState    { _initialState },
        lockOutPeriod  { _lockOutPeriod },
        lastChangeTime { 0 }
    {
    }

    bool registerInterrupt(uint8_t when, bool rawState) {
        if ((uint32_t)(when - lastChangeTime) > lockOutPeriod) {
            if (rawState == stableState)
                return false;
            stableState = rawState;
            lastChangeTime = when;
            return true;
        }
        // ignore if still in lockout period
        return false;
    }

    bool status() {
        return stableState;
    }



};


