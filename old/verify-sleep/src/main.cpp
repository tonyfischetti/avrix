#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include <util/atomic.h>

#include "attiny85-hal.hpp"


using LED1 = GPIO<5>;
using DIN  = GPIO<3>;

volatile bool    flicker_p    { false };
volatile uint8_t current_rand { 0 };

volatile uint32_t tick_counter { 0 };


ISR(WDT_vect) {
    flicker_p = true;
    // tickCounter++;
}


void reset_watchdog() {
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

void go_to_sleep() {
    cli();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
}


int main() {

    // power_adc_disable();
    // power_timer0_disable();
    // power_timer1_disable();
    // power_usi_disable();
    power_all_disable();

    /*
    // Pin Change Interrupt on PB1 (pin 6)
    pinMode(1, INPUT_PULLUP);        // Button input
    GIMSK |= (1 << PCIE);            // Enable pin change interrupts
    PCMSK |= (1 << PCINT1);          // Enable PB1 (PCINT1)
    */

    LED1::setOutput();
    LED1::setLow();
    LED1::pullUp();
    
    reset_watchdog();

    while (1) {

        if (DIN::isHigh()) { LED1::setHigh(); }
        else { LED1::setLow(); }

        reset_watchdog();
        // go_to_sleep();
    }

    return 0;
}


