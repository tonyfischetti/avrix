#include "avr-hal.hpp"

#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>

#include <util/atomic.h>
#include <util/delay.h>

#include "util/TransitionDebouncer.hpp"
#include "util/LFSR.hpp"



using GATE = HAL::GPIO::GPIO<5>;
using LED1 = HAL::GPIO::GPIO<2>;
using LED2 = HAL::GPIO::GPIO<6>;
using LED3 = HAL::GPIO::GPIO<7>;
using WD   = HAL::Watchdog::Watchdog<14>;

using lfsr = HAL::Utils::Random::LFSR;

HAL::Utils::TransitionDebouncer<3> sw(HIGH, 30, true);


volatile bool timeToFlickerP  { false };
volatile uint8_t previousPINB { 0xFF  };
uint8_t mode                  { 0x00  };

volatile bool pin3ChangedP { false };
uint32_t lastChange { 0 };



void changeMode(bool forward = true) {
    if (forward)
        mode = static_cast<uint8_t>((mode + 1) % 3);
    else 
        mode = static_cast<uint8_t>((mode - 1) % 3);
    //  TODO  use switch/case
    if (!mode) {
        GATE::setLow();
        WD::reset();
    } else if (mode == 1) {
        GATE::setLow();
        LED1::setHigh();
        LED2::setHigh();
        LED3::setHigh();
        WD::disable();
    } else {
        LED1::setLow();
        LED2::setLow();
        LED3::setLow();
        GATE::setHigh();
        WD::disable();
    }
}


ISR(WDT_vect) {
    timeToFlickerP = true;
}

ISR(PCINT0_vect) {
    uint8_t current = PINB;
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t changed = current ^ previousPINB;
    previousPINB = current;

    if (changed & (1 << 4)) {
        sw.notifyInterruptOccurred(now, current);
    }
}


void flicker() {
    static uint8_t currentRand { 0 };
    timeToFlickerP = false;

    currentRand = static_cast<uint8_t>(lfsr::nextByte());

    if ((currentRand & (0x03) << 0)) LED1::setHigh();
    else                             LED1::setLow();
    if ((currentRand & (0x03) << 2)) LED2::setHigh();
    else                             LED2::setLow();
    if ((currentRand & (0x03) << 4) ||
       !(currentRand & (0x0f)))      LED3::setHigh();
    else                             LED3::setLow();
}


void start_sequence() {
    _delay_ms(500); LED1::setHigh();
    _delay_ms(500); LED2::setHigh();
    _delay_ms(500); LED3::setHigh();
    _delay_ms(500); LED1::setLow(); LED2::setLow(); LED3::setLow();
}




int main() {

    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    GATE::setOutput();

    start_sequence();

    lfsr::init(93);

    HAL::Ticker::setupMSTimer();
    WD::reset();
    sei();

    while (1) {


        switch (sw.processAnyInterrupts()) {
            case HAL::Transition::RISING:
                break;
            case HAL::Transition::FALLING:
                changeMode();
                break;
            case HAL::Transition::NONE:
                break;
            default:
                break;
        }

        if (!mode) {
            if (timeToFlickerP) flicker();
            WD::reset();
        }

        HAL::Sleep::goToSleep(SLEEP_MODE_IDLE);

    }

    return 0;
}

