#include "../attiny85-hal/include/hal.hpp"

#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "PCINTDebouncer.hpp"



using GATE = HAL::GPIO::GPIO<5>;
using LED1 = HAL::GPIO::GPIO<2>;
using LED2 = HAL::GPIO::GPIO<6>;
using LED3 = HAL::GPIO::GPIO<7>;
using WD   = HAL::Watchdog::Watchdog<14>;

HAL::GPIO::GPIO<3> SW;
PCINTDebouncer<HAL::GPIO::GPIO<3>> SW_D(SW, true);


volatile bool flicker_time_p    { false };
volatile uint8_t previous_pinb  { 0xFF  };
uint8_t mode                    { 0x00  };

volatile bool pin_3_changed { false };

ISR(PCINT0_vect) {
    __asm__ __volatile__ ("" ::: "memory");
    for (volatile uint8_t i = 0; i < 10; ++i);
    uint8_t current = PINB;
    uint8_t changed = current ^ previous_pinb;
    previous_pinb = current;

    //  TODO  hardcoded for debugging
    if (changed & (1 << 3)) {
        pin_3_changed = true;
        // LED1::toggle();
        // SW_D.onISRTriggered((current & (1 << 3)) != 0);
    }

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


int main() {

    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    GATE::setOutput();

    SW.setInput();
    SW.setPullup();
    SW.enablePCINT();

    start_sequence();
    HAL::Ticker::setup_ms_timer();
    sei();

    while (1) {

        if (pin_3_changed) {
            pin_3_changed = false;
            LED1::toggle();
            SW_D.onISRTriggered((PINB & (1 << 3)) != 0);
        }

        if (SW_D.hasUpdate()) {
            if (!SW_D.state())
                LED3::toggle();
        }

    }

    return 0;
}
