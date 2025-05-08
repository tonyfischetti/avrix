#include "avr-hal.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.hpp"
#include "util/TransitionDebouncer.hpp"



volatile uint32_t lastPressed { 0     };
volatile bool watchdogBarkedP { false };
volatile uint8_t previousPIND { 0xFF  };
volatile uint8_t previousPINB { 0xFF  };

volatile bool buttonPressedP  { false };
volatile bool reChangedP { false };


uint8_t numLEDs { 0 };


using LED0 = HAL::GPIO::GPIO<15>;
using LED1 = HAL::GPIO::GPIO<25>;
using LED2 = HAL::GPIO::GPIO<24>;
using LED3 = HAL::GPIO::GPIO<23>;
using MD1  = HAL::GPIO::GPIO<12>;
using MD2  = HAL::GPIO::GPIO<13>;
using MD3  = HAL::GPIO::GPIO<26>;
using GATE = HAL::GPIO::GPIO<11>;

// using WD   = HAL::Watchdog::Watchdog<19>;

HAL::GPIO::GPIO<4>  RE_SW;
HAL::GPIO::GPIO<5>  RE_CLK;
HAL::GPIO::GPIO<6>  RE_DT;
HAL::GPIO::GPIO<14> RAW_BTN;


HAL::Utils::TransitionDebouncer<4> sw(RE_SW, HIGH, 30);
HAL::Utils::TransitionDebouncer<5> clk(RE_CLK, HIGH, 1);
HAL::Utils::TransitionDebouncer<14> btn(RAW_BTN, HIGH, 3);



ISR(PCINT2_vect) {
    uint8_t current = PIND;
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t changed = current ^ previousPIND;
    previousPIND = current;

    if (changed & (1 << 2)) {
        if (!sw.lastUnprocessedInterrupt) {
            sw.lastUnprocessedInterrupt = now;
        }
    }

    if (changed & (1 << 3)) {
        if (!clk.lastUnprocessedInterrupt) {
            clk.lastUnprocessedInterrupt = now;
        }
    }

    
}


ISR(PCINT0_vect) {
    uint8_t current = PINB;
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t changed = current ^ previousPIND;
    previousPINB = current;

    if (changed & 0x01) {
        if (!btn.lastUnprocessedInterrupt) {
            btn.lastUnprocessedInterrupt = now;
        }
    }
}



// ISR(WDT_vect) {
//     watchdogBarkedP = true;
// }



static char alice[] = "::alice glass:: HI!\n";


void start_sequence() {
    _delay_ms(250); LED1::setHigh();
    _delay_ms(250); LED2::setHigh();
    _delay_ms(250); LED3::setHigh();
    _delay_ms(250); LED1::setLow(); LED2::setLow(); LED3::setLow();
}




int main(void) {


    HAL::UART::init<9600>();

    RE_SW.setInputPullup();
    RE_CLK.setInputPullup();
    RE_DT.setInputPullup();
    RAW_BTN.setInputPullup();
    LED0::setOutput();
    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    MD1::setInput(); // pulled-down
    MD2::setInput(); // pulled-down
    MD3::setInput(); // pulled-down


    RE_SW.enablePCINT();
    RE_CLK.enablePCINT();
    RAW_BTN.enablePCINT();
    
    // WD::reset();
    
    HAL::Ticker::setupMSTimer();


    sei();

    start_sequence();

    HAL::UART::print(alice);


    while (1) {

        // TODO 
        // TODO 
        // only big problems left are now:
        //   - rising is laggy (too long weight)
        //   - no <4> nonsense
        //   - privatize lastUnprocessedInterrupt
        switch (sw.processAnyInterrupts()) {
            case HAL::Transition::RISING:
                // LED0::toggle();
                break;
            case HAL::Transition::FALLING:
                LED0::toggle();
                break;
            default:
                break;
        }

        switch (clk.processAnyInterrupts()) {
            case HAL::Transition::RISING:
                break;
            case HAL::Transition::FALLING:
                numLEDs = (numLEDs + 1) % 4;
                break;
            default:
                break;
        }

        switch (btn.processAnyInterrupts()) {
            case HAL::Transition::RISING:
                break;
            case HAL::Transition::FALLING:
                GATE::toggle();
                break;
            default:
                break;
        }

        LED1::setLow();
        LED2::setLow();
        LED3::setLow();
        if (numLEDs > 2) 
            LED3::setHigh();
        if (numLEDs > 1)
            LED2::setHigh();
        if (numLEDs > 0)
            LED1::setHigh();


        // doesn't work
        // HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);


    }
    return 0;
}

