#pragma once

#include <stdint.h>
#include <util/atomic.h>
#include <avr/io.h>

namespace HAL {
namespace Ticker {
    void setup_ms_timer();
    uint32_t get_ticks();
}
}
