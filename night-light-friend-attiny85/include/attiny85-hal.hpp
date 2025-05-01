#pragma once

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


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
    static constexpr uint8_t mask { get_pin_mask(pinNumber) };

    static constexpr void setHigh()   { PORTB |=  mask; }
    static constexpr void setPullup() { PORTB |=  mask; }
    static constexpr void setLow()    { PORTB &= ~mask; }
    static constexpr void setOutput() { DDRB  |=  mask; }
    static constexpr void setInput()  { DDRB  &= ~mask; }
    static constexpr void toggle()    { DDRB  ^=  mask; }

    static constexpr bool read()      { return PINB & mask; }

    static constexpr void enablePCINT() {
        GIMSK |= (1 << PCIE);
        PCMSK |= (1 << PCINT4);
    }
};


inline void go_to_sleep(uint8_t mode) {
    cli();
    set_sleep_mode(mode);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
}
