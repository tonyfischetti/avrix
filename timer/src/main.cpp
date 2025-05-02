#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "attiny85-hal.hpp"


using LED1 = GPIO<2>;
using LED2 = GPIO<6>;
using LED3 = GPIO<7>;

volatile uint32_t ticks { 0 };

ISR(TIM0_COMPA_vect) {
    ticks++;
}

uint32_t get_ticks() {
  uint32_t value;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    value = ticks;
  }
  return value;
}

void start_sequence() {
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


void setup_timer0_1ms() {
    TCCR0A = (1 << WGM01);      // CTC mode
    TCCR0B = (1 << CS01);
    OCR0A = 124;
    TIMSK |= (1 << OCIE0A);     // Enable compare match A interrupt
}


int main() {


    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();

    start_sequence();
    setup_timer0_1ms();
    sei();

    uint32_t last_toggle_tick = 0;

    while (1) {
        uint32_t now = get_ticks();
        if (now - last_toggle_tick >= 1000) {
            last_toggle_tick = now;
            LED1::toggle();
            LED2::toggle();
            LED3::toggle();
        }
    }

    return 0;
}

