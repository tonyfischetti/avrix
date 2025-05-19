#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "avril.hpp"
#include "util/IntTransitionDebouncer.hpp"
#include "util/LFSR.hpp"


using GATE = HAL::GPIO::GPIO<5>;
using LED1 = HAL::GPIO::GPIO<2>;
using LED2 = HAL::GPIO::GPIO<6>;
using LED3 = HAL::GPIO::GPIO<7>;
using WD   = HAL::Watchdog::Watchdog<14>;

using lfsr = HAL::Utils::Random::LFSR;


constexpr uint8_t NUM_MODES { 3 };

volatile bool timeToFlickerP  { false };
volatile uint8_t previousPINB { 0xFF  };
volatile bool pin3ChangedP    { false };
uint8_t mode                  { 0x00  };

HAL::Utils::IntTransitionDebouncer<3, 30, HIGH, true> sw;


void changeMode() {
    mode = static_cast<uint8_t>((mode + 1) % NUM_MODES);
    switch (mode) {
        case 0:
            GATE::setLow();
            WD::reset();
            break;
        case 1:
            GATE::setLow();
            LED1::setHigh();
            LED2::setHigh();
            LED3::setHigh();
            WD::disable();
            break;
        case 2:
            LED1::setLow();
            LED2::setLow();
            LED3::setLow();
            GATE::setHigh();
            WD::disable();
            break;
        default:
            break;
    }
}


ISR(WDT_vect) {
    timeToFlickerP = true;
}

ISR(PCINT0_vect) {
    //  TODO  why, if I set this to 0, does it not work until 3 clicks?
    //  TODO  and, when I don't use the onFalling callback, it's only 1
    HAL::Ticker::resume(1);
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t current = PINB;
    uint8_t changed = current ^ previousPINB;
    previousPINB = current;

    sw.notifyInterruptOccurred(now, changed);
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

    power_adc_disable();
    power_timer1_disable();

    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    GATE::setOutput();

    lfsr::init(93);

    HAL::Ticker::setupMSTimer();
    WD::reset();
    sw.begin();
    sei();

    start_sequence();

    sw.setOnFalling(&changeMode);

    while (1) {
        sw.processAnyInterrupts();

        if (!mode) {
            if (timeToFlickerP) flicker();
            WD::reset();
        }

        if (!sw.pendingDebounceTimeout()) {
            HAL::Ticker::pause();
            HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);
        }
    }

    return 0;
}

