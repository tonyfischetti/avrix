#include "../attiny85-hal/include/hal.hpp"

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>



using LED1 = HAL::GPIO::GPIO<2>;
using LED2 = HAL::GPIO::GPIO<6>;
using LED3 = HAL::GPIO::GPIO<7>;



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




int main() {

    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();

    start_sequence();
    HAL::Ticker::setup_ms_timer();
    sei();

    uint32_t last_toggle_tick = 0;

    while (1) {
        uint32_t now = HAL::Ticker::get_ticks();
        if (now - last_toggle_tick >= 1000) {
            last_toggle_tick = now;
            LED1::toggle();
            LED2::toggle();
            LED3::toggle();
        }
    }

    return 0;
}

