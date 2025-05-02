#include <stdint.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "attiny85-hal.hpp"
#include "Debouncer.hpp"


using GATE = GPIO<5>;
using LED1 = GPIO<2>;
using LED2 = GPIO<6>;
using LED3 = GPIO<7>;
using WD   = Watchdog<14>;
GPIO<3> SW;
Debouncer<GPIO<3>, uint32_t> SW_D(SW, false, 1);


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
    static uint8_t current_rand { 0 };

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
    } else if (mode == 1) {
        GATE::setLow();
        LED1::setHigh();
        LED2::setHigh();
        LED3::setHigh();
    } else {
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

    SW.setInput();
    SW.setPullup();
    SW.enablePCINT();

    WD::reset();

    while (1) {
        if (watchdog_barked_p) {
            SW_D.update(get_tick_counter());
            watchdog_barked_p = false;
            if (!mode) {
                flicker();
            }
            WD::reset();
        }

        if (SW_D.isActiveP()) {
            move_mode_forward();
        }

        go_to_sleep(SLEEP_MODE_PWR_DOWN);
    }

    return 0;
}

