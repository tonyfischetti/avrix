#ifndef PIN_H
#define PIN_H

#include <avr/io.h>

template<uint8_t bit>
struct Pin {
    static void setOutput() { DDRB  |=  (1 << bit); }
    static void setInput()  { DDRB  &= ~(1 << bit); }
    static void pullUp()    { PORTB |=  (1 << bit); }
    static void high()      { PORTB |=  (1 << bit); }
    static void low()       { PORTB &= ~(1 << bit); }
    static bool read()      { return !(PINB & (1 << bit)); }
};

#endif
