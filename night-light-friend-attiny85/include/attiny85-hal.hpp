#pragma once

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>


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
    static constexpr void toggle()    { PORTB ^=  mask; }
    static constexpr void setOutput() { DDRB  |=  mask; }
    static constexpr void setInput()  { DDRB  &= ~mask; }

    static constexpr bool read()      { return PINB & mask; }

    static constexpr void enablePCINT() {
        GIMSK |= (1 << PCIE);
        PCMSK |= mask;
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


inline void reset_watchdog() {
	cli();

    // MCUSR - MCU Status Register
    MCUSR &= static_cast<uint8_t>(~(1 << WDRF));
    // WDRF = 3 - clear the WDT reset flag

    // WDTCR - Watchdog Timer Control Register
    WDTCR |= (1 << WDCE) | (1 << WDE);  // needed to change prescaler bits
    // WDCE = 4 - Watchdog Change Enable (needed to change prescaler bits)
    // WDE  = 3 - Watchdog Enable
    // WDIE = 6 - Watchdog Timeout Interrupt Enable

    // pre-scaler bits
    // WDP3 = 5
    // WDP2 = 2 
    // WDP1 = 1
    // WDP0 = 0

    // WDTCR = (1 << WDIE) | (1 << WDP1);    // 64 ms
    WDTCR = (1 << WDIE) | (1 << WDP1) | (1 << WDP0); // 125 ms
    // WDTCR = (1 << WDIE) | (1 << WDP2); // 250 ms
    // WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1); // 1 second
    // WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1) | (1 << WDP0); // 2 seconds

	// Enable all interrupts.
	sei();
}

inline void disable_watchdog() {
    cli();                          // Disable interrupts
    MCUSR &= ~(1 << WDRF);          // Clear Watchdog reset flag
    WDTCR |= (1 << WDCE) | (1 << WDE); // Start timed sequence
    WDTCR = 0x00;                  // Disable WDT
    sei();                          // Enable interrupts
}

