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


constexpr uint16_t STEP_DELAY { 100 };

volatile uint8_t currentRand { 0 };


ISR(WDT_vect) {
    PORTB = 0;

    currentRand = randomIntFromUpTo(0, 63);

    if ((currentRand & (0x03) << 0)) LED1::setHigh();
    currentRand = randomIntFromUpTo(0, 63);
    if ((currentRand & (0x03) << 2)) LED2::setHigh();
    currentRand = randomIntFromUpTo(0, 63);
    if ((currentRand & (0x03) << 4)) LED3::setHigh();
    // if ((currentRand & (0x03) << 4) ||
    //      !currentRand) LED3::setHigh();

    // reset watchdog
    WDTCR |= (1<<WDIE);
}


int main() {

    power_adc_disable();
    power_timer0_disable();
    power_timer1_disable();
    power_usi_disable();

	cli();

    MCUSR &= ~(1 << WDRF);                // Clear WDT reset flag
    WDTCR |= (1 << WDCE) | (1 << WDE);    // Enable configuration mode
    // WDTCR = (1 << WDIE) | (1 << WDP2);    // Interrupt mode, 125ms
    WDTCR = (1 << WDIE) | (1 << WDP1);    // 64 ms

	// Enable all interrupts.
	sei();

    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();

    while (1) {
        sleep_mode();
    }

    return 0;
}

// 1867    build/main.hex
// 1ee7dca0346c22149a128819d7683a4d  build/main.hex
