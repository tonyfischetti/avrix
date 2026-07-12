#pragma once

#include <stdint.h>

struct Pattern {
    virtual uint16_t tick() = 0;
    virtual void onCW() { };
    virtual void onCCW() { };
    virtual void onRelease() { };

  protected:
    // patterns are static objects, never deleted through a Pattern*;
    // a protected non-virtual dtor enforces that without dragging
    // operator delete into the binary like a virtual dtor would
    ~Pattern() = default;
};
