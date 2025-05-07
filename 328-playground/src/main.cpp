#include "avr-hal.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "PCINTDebouncer.hpp"
#include "uart.hpp"


volatile uint32_t lastPressed { 0     };
volatile bool watchdogBarkedP { false };
volatile uint8_t previousPIND { 0xFF  };

volatile bool buttonPressedP  { false };
volatile bool reChangedP { false };


uint8_t numLEDs { 0 };


using LED0    = HAL::GPIO::GPIO<15>;
using LED1    = HAL::GPIO::GPIO<25>;
using LED2    = HAL::GPIO::GPIO<24>;
using LED3    = HAL::GPIO::GPIO<23>;
using MD1     = HAL::GPIO::GPIO<12>;
using MD2     = HAL::GPIO::GPIO<13>;
using MD3     = HAL::GPIO::GPIO<26>;
// using RE_CLK  = HAL::GPIO::GPIO<5>;
using RE_DATA = HAL::GPIO::GPIO<6>;
using Button  = HAL::GPIO::GPIO<14>;

// using WD   = HAL::Watchdog::Watchdog<19>;

HAL::GPIO::GPIO<4> RE_SW;
HAL::GPIO::GPIO<5> RE_CLK;

enum class Transition : uint8_t { RISING, FALLING, NONE };


template<uint8_t physicalPin>
class TransitionDebouncer {

    // fix this <4> nonsense
    const HAL::GPIO::GPIO<physicalPin>& gpio;
    // parameterize
    const uint32_t debounceWaitTime;
    bool stableState;

  public:
    volatile uint32_t lastUnprocessedInterrupt;

    TransitionDebouncer(HAL::GPIO::GPIO<physicalPin>& _gpio, bool _initial, uint32_t _debounceWaitTime)
        : gpio { _gpio },
          debounceWaitTime { _debounceWaitTime },
          stableState { _initial },
          lastUnprocessedInterrupt { 0 } {
    }

    Transition processAnyInterrupts() {
        Transition transistion { Transition::NONE };
        if (lastUnprocessedInterrupt > 0) {
            uint32_t now = HAL::Ticker::getNumTicks();
            if (((uint32_t)(now - lastUnprocessedInterrupt)) >= debounceWaitTime) {
                bool nowState = gpio.read();

                if (nowState && !stableState)
                    transistion = Transition::RISING;
                else if (!nowState && stableState)
                    transistion = Transition::FALLING;

                stableState = nowState;

                lastUnprocessedInterrupt = 0;
            }
        }
        return transistion;
    }
};

TransitionDebouncer<4> sw(RE_SW, LOW, 30);
TransitionDebouncer<5> clk(RE_CLK, LOW, 1);



ISR(PCINT2_vect) {
    uint8_t current = PIND;
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t changed = current ^ previousPIND;
    previousPIND = current;

    // the pin changed in some way since last interrupt
    if (changed & (1 << 2)) {
        // if there were a previous interrupt and it was not handled outside
        // the ISR, we don't want to update the interrupt time
        //
        // the idea is to wait _x_ amount of milliseconds _after_ the
        // first PCINT (bounce 0) and then take the level then and
        // treat it as fact
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
    RE_DATA::setInputPullup();
    LED0::setOutput();
    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    MD1::setInput(); // pulled-down
    MD2::setInput(); // pulled-down
    MD3::setInput(); // pulled-down


    RE_SW.enablePCINT();
    RE_CLK.enablePCINT();
    // RE_CLK.enablePCINT();
    
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
            case Transition::RISING:
                LED0::toggle();
                break;
            case Transition::FALLING:
                break;
            default:
                break;
        }

        switch (clk.processAnyInterrupts()) {
            case Transition::RISING:
                break;
            case Transition::FALLING:
                numLEDs = (numLEDs + 1) % 4;
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

