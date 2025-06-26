#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "avril.hpp"


using NEO = HAL::GPIO::GPIO<5>;

extern "C" {
    void sendbyte(uint8_t b);
}


int main() {

    power_adc_disable();
    power_timer1_disable();

    NEO::setOutput();

    // HAL::Ticker::setupMSTimer();
    sei();

    _delay_ms(500);


    while (1) {

        cli();

        sendbyte(0);

        _delay_ms(25);

        sendbyte(255);

        _delay_ms(100);

    }
}

