#pragma once

#include <stdint.h>

struct Pattern {
    virtual uint16_t tick() = 0;
    virtual void onCW() { };
    virtual void onCCW() { };
    virtual void onRelease() { };

};
