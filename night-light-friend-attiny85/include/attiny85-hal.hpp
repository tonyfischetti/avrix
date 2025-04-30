#pragma once
#include <avr/io.h>

static constexpr uint8_t getMask(uint8_t pnum) {
    if      (pnum == 5) return (1 << 0); // PB0
    else if (pnum == 6) return (1 << 1); // PB1
    else if (pnum == 7) return (1 << 2); // PB2
    else if (pnum == 2) return (1 << 3); // PB3
    else if (pnum == 3) return (1 << 4); // PB4
    else                return (1 << 0); // ???
}

template<uint8_t pinNumber>
struct GPIO {
    static constexpr uint8_t mask { getMask(pinNumber) };

    static constexpr void setHigh()   { PORTB |=  mask; }
    static constexpr void setLow()    { PORTB &= ~mask; }
    static constexpr void setOutput() { DDRB  |=  mask; }
    static constexpr void setInput()  { DDRB  &= ~mask; }
    static constexpr void toggle()    { DDRB  ^=  mask; }
};

