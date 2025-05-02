#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "attiny85-hal.hpp"


using GATE = GPIO<5>;
using SW   = GPIO<3>;
using LED1 = GPIO<2>;
using LED2 = GPIO<6>;
using LED3 = GPIO<7>;
using WD   = Watchdog<19>;


volatile uint32_t tick_counter      { 0 };
volatile bool watchdog_barked_p     { false };
uint8_t mode                        { 0x00 };



ISR(WDT_vect) {
    watchdog_barked_p = true;
    tick_counter++;
}

ISR(PCINT0_vect) {
  // wake up
}


uint32_t get_tick_counter() {
  uint32_t value;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    value = tick_counter;
  }
  return value;
}


void flicker() {
}

void start_sequence() {
    _delay_ms(1000);
    LED1::setHigh();
    _delay_ms(1000);
    LED2::setHigh();
    _delay_ms(1000);
    LED3::setHigh();
    _delay_ms(1000);
    LED1::setLow();
    LED2::setLow();
    LED3::setLow();
}

void restart_sequence() {
    _delay_ms(500);
    LED1::setHigh();
    _delay_ms(500);
    LED2::setHigh();
    _delay_ms(500);
    LED3::setHigh();
    _delay_ms(500);
    LED1::setLow();
    LED2::setLow();
    LED3::setLow();
}

int main() {

    power_all_disable();


    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    GATE::setOutput();

    SW::setInput();
    SW::setPullup();
    SW::enablePCINT();

    bool onP { false };

    start_sequence();

    if (MCUSR & (1 << WDRF)) {
        // Show watchdog reset indicator (e.g. turn LED on for 2 seconds)
        restart_sequence();
    }

    WD::reset();
    sei();

    uint8_t counter { 0 };


    while (1) {
        if (onP)
            LED1::setHigh();
        else
            LED1::setLow();
        onP = !onP;
        _delay_ms(1000);

        //
        // for (uint8_t i = 0; i<counter; i++) {
        //     _delay_ms(1000);
        // }
        // counter++;
        //
        // wdt_reset();
    }

    return 0;
}

