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
// PCINTDebouncer RE_SW_D(true, 20);

// HAL::GPIO::GPIO<5> RE_CLK;
// PCINTDebouncer RE_CLK_D(true, 20);


struct LatestRawEvent {
    volatile bool rawState;
    volatile uint32_t when;
};

struct FallingDebouncer {
    volatile bool stableState;
    volatile struct LatestRawEvent latestRawEvent;
    volatile uint32_t lastStateChange;
    // parameterize type
    volatile uint16_t timeOutPeriod;
    volatile uint32_t wasInterrupted;
};

static struct LatestRawEvent tmp = { 1, 0 };
static struct FallingDebouncer SW_FD = { true, tmp, 0, 30, 0 };



struct DBouncer {
    const HAL::GPIO::GPIO<4>& gpio;
    // parameterize
    const uint32_t debounceWaitTime;
    volatile uint32_t lastUnprocessedInterrupt;
    // needs to be volatile?
    bool stableState;

    DBouncer(HAL::GPIO::GPIO<4>& _gpio, uint32_t _debounceWaitTime)
        : gpio { _gpio },
          debounceWaitTime { _debounceWaitTime },
          // for now
          stableState { true },
          lastUnprocessedInterrupt { 0 } {
    }
};

DBouncer swnew(RE_SW, 75);



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
        if (!swnew.lastUnprocessedInterrupt) {
            swnew.lastUnprocessedInterrupt = now;
        }
    }
}





// ISR(WDT_vect) {
//     watchdogBarkedP = true;
// }



static char alice[] = "::alice glass:: HI!\r\n";
static char es[] = "exit setup\r\n";
static char yes[] = "p!\r\n";
static char no[] = "!p\r\n";
static char period[] = ".";
static char newline[] = "\n";


void start_sequence() {
    _delay_ms(250);
    LED1::setHigh();
    _delay_ms(250);
    LED2::setHigh();
    _delay_ms(250);
    LED3::setHigh();
    _delay_ms(250);
    LED1::setLow();
    LED2::setLow();
    LED3::setLow();
}




int main(void) {


    HAL::UART::init<9600>();

    RE_SW.setInputPullup();
    // RE_CLK.setInputPullup();
    RE_DATA::setInputPullup();
    LED0::setOutput();
    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    MD1::setInput(); // pulled-down
    MD2::setInput(); // pulled-down
    MD3::setInput(); // pulled-down


    RE_SW.enablePCINT();
    // RE_CLK.enablePCINT();
    
    // WD::reset();
    
    HAL::Ticker::setupMSTimer();


    sei();

    start_sequence();

    HAL::UART::print(alice);




    while (1) {

        // if (swnew.newState()) {

        if (swnew.lastUnprocessedInterrupt > 0) {
            uint32_t when = swnew.lastUnprocessedInterrupt;
            uint32_t now = HAL::Ticker::getNumTicks();
            if (((uint32_t)(now - when)) >= swnew.debounceWaitTime) {
                bool nowState = swnew.gpio.read();
                if (!nowState) {
                    swnew.stableState = !swnew.stableState;
                }
                swnew.lastUnprocessedInterrupt = 0;
            }
        }


        if (swnew.stableState)
            LED0::setHigh();
        else
            LED0::setLow();


        // doesn't work
        // HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);


    }
    return 0;
}

