#pragma once

#include <stdint.h>


struct Mode {

    Mode() noexcept;

    virtual void update()      noexcept;

    virtual void reCw()      noexcept;
    virtual void reCcw()     noexcept;
    virtual void rePress()   noexcept;

};


