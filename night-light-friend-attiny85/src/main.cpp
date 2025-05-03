#include "../attiny85-hal/include/hal.hpp"

#include <stdint.h>
#include <stdlib.h>

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


    volatile uint32_t ticks = 0;

    void setup_ms_timer() {
        TCCR0A = (1 << WGM01);      // CTC mode
        TCCR0B = (1 << CS01);
        OCR0A = 124;
        TIMSK |= (1 << OCIE0A);     // Enable compare match A interrupt
    }

    uint32_t get_ticks() {
        uint32_t value;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        value = ticks;
        }
        return value;
    }

ISR(TIM0_COMPA_vect) {
    ticks++;
}

ISR(WDT_vect) {
    flicker_time_p = true;
}

ISR(PCINT0_vect) {
    uint8_t current = PINB;
    uint8_t changed = current ^ previous_pinb;
    previous_pinb = current;

    //  TODO  hardcoded for debugging
    if (changed & (1 << 3)) {
        SW_D.onISRTriggered(current & (1 << 3));
    }

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
    mode = static_cast<uint8_t>((mode + 1) % 3);
    //  TODO  use switch/case
    if (!mode) {
        GATE::setLow();
        WD::reset();
    } else if (mode == 1) {
        GATE::setLow();
        // LED1::setHigh();
        // LED2::setHigh();
        // LED3::setHigh();
        WD::disable();
    } else {
        LED1::setLow();
        LED2::setLow();
        LED3::setLow();
        GATE::setHigh();
        WD::disable();
    }
}


int main() {

    // power_adc_disable();
    // power_timer0_disable();
    // power_timer1_disable();
    // power_usi_disable();
    // power_all_disable();


    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    GATE::setOutput();

    SW.setInput();
    SW.setPullup();
    SW.enablePCINT();

    move_mode_forward();
    
    power_timer0_enable();
    // HAL::Ticker::setup_ms_timer();
    // sei();

    WD::reset();


    while (1) {
        if (!mode) {
            if (flicker_time_p) flicker();
            WD::reset();
        }
        
        if (SW_D.hasUpdate()) {

            if (!SW_D.state())
                LED1::toggle();
            // else
            //     LED1::setLow();
            //     // move_mode_forward();
        }

        static uint32_t last_ticks = 0;
        uint32_t now = get_ticks();
        if (now != last_ticks) {
            LED3::toggle();  // This will now blink once per ms
            last_ticks = now;
        }

        // HAL::Sleep::go_to_sleep(SLEEP_MODE_PWR_DOWN);
    }

    return 0;
}

