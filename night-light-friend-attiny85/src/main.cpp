#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "attiny85-hal.hpp"


using GATE = GPIO<5>;

using SW = GPIO<3>;

using LED1 = GPIO<2>;
using LED2 = GPIO<6>;
using LED3 = GPIO<7>;


volatile uint32_t tick_counter  { 0 };
volatile bool flicker_time_p    { false };
volatile bool button_pressed_p  { false };
uint8_t mode                    { 0x00 };



ISR(WDT_vect) {
    flicker_time_p = true;
    tick_counter++;
}

ISR(PCINT0_vect) {
  button_pressed_p = true;
}


uint32_t get_tick_counter() {
  uint32_t value;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    value = tick_counter;
  }
  return value;
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

void flicker() {
    static uint8_t current_rand { 0 };
    flicker_time_p = false;

    current_rand = static_cast<uint8_t>(rand());

    if ((current_rand & (0x03) << 0)) LED1::setHigh();
    else                              LED1::setLow();
    if ((current_rand & (0x03) << 2)) LED2::setHigh();
    else                              LED2::setLow();
    if ((current_rand & (0x03) << 4) ||
       !(current_rand & (0x0f)))      LED3::setHigh();
    else                              LED3::setLow();
}


void move_mode_forward() {
    mode = (mode + 1) % 3;
    //  TODO  use switch/case
    if (!mode) {
        GATE::setLow();
        // install watchdog
    } else if (mode == 1) {
        // uninstall watchdog
        GATE::setLow();
        LED1::setHigh();
        LED2::setHigh();
        LED3::setHigh();
    } else {
        // uninstall watchdog
        LED1::setLow();
        LED2::setLow();
        LED3::setLow();
        GATE::setHigh();
    }
}


int main() {

    // power_adc_disable();
    // power_timer0_disable();
    // power_timer1_disable();
    // power_usi_disable();
    power_all_disable();


    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    GATE::setOutput();

    SW::setInput();
    SW::setPullup();

    SW::enablePCINT();

    reset_watchdog();

    while (1) {
        if (!mode) {
            if (flicker_time_p) flicker();
        }

        if (button_pressed_p) {
            _delay_ms(200);
            button_pressed_p = false;
            move_mode_forward();
        }

        reset_watchdog();
        go_to_sleep(SLEEP_MODE_PWR_DOWN);
    }

    return 0;
}

