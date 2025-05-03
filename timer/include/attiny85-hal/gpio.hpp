#pragma once

#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>


namespace HAL {
namespace GPIO {

static constexpr uint8_t get_pin_mask(uint8_t pnum) {
    if      (pnum == 5) return (1 << 0); // PB0
    else if (pnum == 6) return (1 << 1); // PB1
    else if (pnum == 7) return (1 << 2); // PB2
    else if (pnum == 2) return (1 << 3); // PB3
    else if (pnum == 3) return (1 << 4); // PB4
    else                return (1 << 0); // ???
}

template<uint8_t pinNumber>
struct GPIO {
    static_assert(pinNumber <= 7 && pinNumber >= 2, "invalid pin");

    static constexpr uint8_t mask { get_pin_mask(pinNumber) };

    static inline void setHigh()   { PORTB |=  mask; }
    static inline void setPullup() { PORTB |=  mask; }
    static inline void setLow()    { PORTB &= ~mask; }
    static inline void toggle()    { PORTB ^=  mask; }
    static inline void setOutput() { DDRB  |=  mask; }
    static inline void setInput()  { DDRB  &= ~mask; }

    static inline bool read() {
        return PINB & mask;
    }

    static inline void enablePCINT() {
        GIMSK |= (1 << PCIE);
        PCMSK |= mask;
    }
};

}
}
