#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include "attiny85-hal.hpp"
#include "utils.h"

using LED1 = GPIO<5>;
using LED2 = GPIO<6>;
using LED3 = GPIO<7>;


volatile bool    flickerP    { false };
volatile uint8_t currentRand { 0 };

volatile uint32_t tickCounter { 0 }; // 125ms currently  TODO 


ISR(WDT_vect) {
    flickerP = true;
    // tickCounter++;
}

// example
// ISR(PCINT0_vect) {
//   buttonPressed = true; // Set button flag
// }


/*
uint32_t getTickCounter() {
  uint32_t value;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    value = tickCounter;
  }
  return value;
}
*/


void flicker() {
    flickerP = false;

    PORTB = 0;

    currentRand = rand();

    if ((currentRand & (0x03) << 0)) LED1::setHigh();
    if ((currentRand & (0x03) << 2)) LED2::setHigh();
    if ((currentRand & (0x03) << 4) ||
         !currentRand) LED3::setHigh();
}


int main() {

    // power_adc_disable();
    // power_timer0_disable();
    // power_timer1_disable();
    // power_usi_disable();
    power_all_disable();

	cli();

    // MCUSR - MCU Status Register
    MCUSR &= ~(1 << WDRF);
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

    // WDTCR = (1 << WDIE) | (1 << WDP2) | (1 << WDP1); // 1 second
    WDTCR = (1 << WDIE) | (1 << WDP1) | (1 << WDP0); // 125 ms
    // WDTCR = (1 << WDIE) | (1 << WDP1);    // 64 ms


    /*
    // Pin Change Interrupt on PB1 (pin 6)
    pinMode(1, INPUT_PULLUP);        // Button input
    GIMSK |= (1 << PCIE);            // Enable pin change interrupts
    PCMSK |= (1 << PCINT1);          // Enable PB1 (PCINT1)
    */


	// Enable all interrupts.
	sei();

    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    while (1) {
        if (flickerP) flicker();

        /*
        if (buttonPressed) {
            buttonPressed = false;
            something();
        }
        */

        sleep_enable();
        sleep_cpu();
        sleep_disable();
    }

    return 0;
}


