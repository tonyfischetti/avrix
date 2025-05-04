#pragma once

#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>


namespace HAL {
namespace GPIO {

enum class Port : uint8_t { B, C, D, Invalid };

struct PinInfo {
    Port port;
    uint8_t bit;
    uint8_t pcint_number;
    uint8_t pcicr_bit;
};

constexpr PinInfo pin_table[29] = {
    { Port::Invalid, 0, 0xFF, 0xFF }, //  0
    { Port::Invalid, 0, 0xFF, 0xFF }, //  1 RESET
    { Port::D, 0, 16, PCIE2 },        //  2 PD0
    { Port::D, 1, 17, PCIE2 },        //  3 PD1
    { Port::D, 2, 18, PCIE2 },        //  4 PD2
    { Port::D, 3, 19, PCIE2 },        //  5 PD3
    { Port::D, 4, 20, PCIE2 },        //  6 PD4
    { Port::Invalid, 0, 0xFF, 0xFF }, //  7 VCC
    { Port::Invalid, 0, 0xFF, 0xFF }, //  8 GND
    { Port::Invalid, 0, 0xFF, 0xFF }, //  9 XTAL1
    { Port::Invalid, 0, 0xFF, 0xFF }, // 10 XTAL2
    { Port::D, 5, 21, PCIE2 },        // 11 PD5
    { Port::D, 6, 22, PCIE2 },        // 12 PD6
    { Port::D, 7, 23, PCIE2 },        // 13 PD7
    { Port::B, 0, 0,  PCIE0 },        // 14 PB0
    { Port::B, 1, 1,  PCIE0 },        // 15 PB1
    { Port::B, 2, 2,  PCIE0 },        // 16 PB2
    { Port::B, 3, 3,  PCIE0 },        // 17 PB3
    { Port::B, 4, 4,  PCIE0 },        // 18 PB4
    { Port::B, 5, 5,  PCIE0 },        // 19 PB5
    { Port::Invalid, 0, 0xFF, 0xFF }, // 20 AVCC
    { Port::Invalid, 0, 0xFF, 0xFF }, // 21 AREF
    { Port::Invalid, 0, 0xFF, 0xFF }, // 22 GND
    { Port::C, 0, 8,  PCIE1 },        // 23 PC0
    { Port::C, 1, 9,  PCIE1 },        // 24 PC1
    { Port::C, 2, 10, PCIE1 },        // 25 PC2
    { Port::C, 3, 11, PCIE1 },        // 26 PC3
    { Port::C, 4, 12, PCIE1 },        // 27 PC4
    { Port::C, 5, 13, PCIE1 },        // 28 PC5
};

// resolved at runtime :(
struct PinRegisters {
    volatile uint8_t* ddr;
    volatile uint8_t* port;
    volatile uint8_t* pin;
    volatile uint8_t* pcmsk;
};

inline PinRegisters resolveRegisters(Port port) {
    switch (port) {
        case Port::B: return { &DDRB, &PORTB, &PINB, &PCMSK0 };
        case Port::C: return { &DDRC, &PORTC, &PINC, &PCMSK1 };
        case Port::D: return { &DDRD, &PORTD, &PIND, &PCMSK2 };
        default:      return { nullptr, nullptr, nullptr, nullptr };
    }
}

template<uint8_t physicalPin>
struct GPIO {
    static_assert(physicalPin < 28 || physicalPin > 0,
            "Invalid pin number for ATMega328P"); //  TODO  

    static constexpr PinInfo info = pin_table[physicalPin];
    // static_assert(info.port != Port::Invalid, "Invalid GPIO pin");

    static inline PinRegisters regs = resolveRegisters(info.port);
    static inline uint8_t mask = (1 << info.bit);

    static inline void setOutput() { *(regs.ddr)  |=  mask; }
    static inline void setInput()  { *(regs.ddr)  &= ~mask; }
    static inline void setHigh()   { *(regs.port) |=  mask; }
    static inline void setLow()    { *(regs.port) &= ~mask; }
    static inline void toggle()    { *(regs.port) ^=  mask; }
    static inline bool read()      { return *(regs.pin) & mask; }

    static inline void setInputPullup() { setInput(); setHigh(); }

    static inline void enablePCINT() {
        PCICR |= (1 << info.pcicr_bit);
        *(regs.pcmsk) |= mask;
    }
};

}
}
