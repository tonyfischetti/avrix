
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "avril.hpp"
#include "comms/uart.hpp"
#include "utils/IntTransitionDebouncer.hpp"
// #include "utils/IntButtonDebouncer.hpp"
#include "devices/RotaryEncoder.hpp"
#include "devices/Button.hpp"



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

// the old Watchdog<powOfTwo> API is gone; interrupt mode is now e.g.
// HAL::Watchdog::enableInterrupt<HAL::Watchdog::Timeout::MS4096>();

// HAL::GPIO::GPIO<4>  RE_SW;
// HAL::GPIO::GPIO<5>  RE_CLK;
// HAL::GPIO::GPIO<6>  RE_DT;
// HAL::GPIO::GPIO<14> RAW_BTN;



HAL::Devices::Button<4, 30, 1000, HIGH, true> sw;

// HAL::Utils::IntTransitionDebouncer<5,  1,  HIGH, true> clk;
HAL::Devices::RotaryEncoder<5, 6, true> clk;
HAL::Utils::IntTransitionDebouncer<14, 3,  HIGH, true> btn;


// one ISR per port, each passing its Port so the devices can filter:
// bit positions collide across ports, and on the 328P every port has
// its own PCINT vector
ISR(PCINT2_vect) {
    uint8_t current = PIND;
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t changed = current ^ previousPIND;
    previousPIND = current;

    sw.notifyInterruptOccurred(now, HAL::GPIO::Port::D, changed);

    clk.notifyInterruptOccurred(now, HAL::GPIO::Port::D, changed);

}


ISR(PCINT0_vect) {
    uint8_t current = PINB;
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t changed = current ^ previousPINB;
    previousPINB = current;

    btn.notifyInterruptOccurred(now, HAL::GPIO::Port::B, changed);
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


    HAL::Comms::UART::init<9600>();

    // RE_DT.setInputPullup();
    LED0::setOutput();
    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    MD1::setInput(); // pulled-down
    MD2::setInput(); // pulled-down
    MD3::setInput(); // pulled-down

    HAL::Ticker::setupMSTimer();

    sw.begin();
    clk.begin();
    btn.begin();
    // HAL::Watchdog::enableInterrupt<HAL::Watchdog::Timeout::MS4096>();
    
    sei();

    start_sequence();

    HAL::Comms::UART::print(alice);

    sw.setOnLongPress([]() { GATE::toggle(); });

    // sw.setOnPress(&gateOn);
    // sw.setOnRelease(&gateOff);
    sw.setOnRelease([]() { numLEDs = (numLEDs + 1) % 5; });

    clk.setOnCW( []() { if (numLEDs != 4) numLEDs++; });
    clk.setOnCCW([]() { if (numLEDs != 0) numLEDs--; });

    while (1) {

        clk.process();

        sw.process();

        /*
        switch (clk.process()) {
            case HAL::RotaryEncoderAction::CW:
                if (numLEDs != 4)
                    numLEDs++;
                break;
            case HAL::RotaryEncoderAction::CCW:
                if (numLEDs != 0)
                    numLEDs--;
                break;
            default:
                break;
        }
        */

        /*
        switch (sw.process()) {
            case HAL::ButtonAction::RELEASE:
                break;
            case HAL::ButtonAction::PRESS:
                break;
            case HAL::ButtonAction::LONG_PRESS:
                LED0::toggle();
                // GATE::toggle();
                break;
            default:
                break;
        }
        */

        /*
        switch (clk.processAnyInterrupts()) {
            case HAL::Transition::RISING:
                break;
            case HAL::Transition::FALLING:
                numLEDs = (numLEDs + 1) % 5;
                break;
            default:
                break;
        }
        */

        switch (btn.processAnyInterrupts()) {
            case HAL::Transition::RISING:
                break;
            case HAL::Transition::FALLING:
                GATE::toggle();
                break;
            case HAL::Transition::NONE:
                break;
            default:
                break;
        }

        LED0::setLow();
        LED1::setLow();
        LED2::setLow();
        LED3::setLow();
        if (numLEDs > 3)
            LED0::setHigh();
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

