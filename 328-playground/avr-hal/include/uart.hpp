#pragma once

#include "hal_common.hpp"

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


namespace HAL {
namespace UART {

#if defined(__AVR_ATtiny85__)
#warning "HAL::UART is not supported on ATtiny85"

static void init() {}
static void write(uint8_t) {}

}
#else

static void init() {
    // UART atmega328p setup
}

static void write(uint8_t byte) {
    // ...
}

#endif

}
}
