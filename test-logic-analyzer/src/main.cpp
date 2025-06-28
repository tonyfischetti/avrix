#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "avril.hpp"
#include "drivers/Button.hpp"
#include "drivers/RotaryEncoder.hpp"


extern "C" {
    void flip();
    void sendbyte(uint8_t b);
}


using NEO = HAL::GPIO::GPIO<5>;
using LED = HAL::GPIO::GPIO<6>;
HAL::Drivers::Button<3, 30, 1000, HIGH, true>     sw;
HAL::Drivers::RotaryEncoder<7, 2, 0, HIGH, true> clk;


volatile uint8_t previousPINB	{  0xFF };
volatile uint8_t abortTxP	    { false };
static   uint8_t numRows        {     1 };
static   uint8_t brightness     {     0 };



void addAnotherRow() {
    // LED::setHigh();
    numRows = (numRows + 1) % 5;
}


/**
 * 4.875 us
ISR(PCINT0_vect) {
    PORTB = 0x01;
    HAL::Ticker::resume(1);
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t current = PINB;
    uint8_t changed = current ^ previousPINB;
    previousPINB = current;

    sw.notifyInterruptOccurred(now, changed);
    clk.notifyInterruptOccurred(now, changed);
    PORTB = 0x00;
}
*/

ISR(PCINT0_vect) {
    abortTxP = true;
    HAL::Ticker::resume(1);
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t current = PINB;
    uint8_t changed = current ^ previousPINB;
    previousPINB = current;

    sw.notifyInterruptOccurred(now, changed);
    clk.notifyInterruptOccurred(now, changed);
}

void increaseBrightness() {
    if (brightness > 245)
        brightness = 255;
    else
        brightness += 5;
}

void decreaseBrightness() {
    if (brightness < 5)
        brightness = 0;
    else
        brightness -= 5;
}


void sendPixel(uint8_t red,
               uint8_t green,
               uint8_t blue,
               uint8_t warm) {
    sendbyte(green);
    sendbyte(red);
    sendbyte(blue);
    sendbyte(warm);
}



int main() {

    power_adc_disable();
    power_timer1_disable();

    NEO::setOutput();
    LED::setOutput();

    HAL::Ticker::setupMSTimer();

    sw.begin();
    clk.begin();

    sei();


    sw.setOnRelease(&addAnotherRow);
    clk.setOnCW(&increaseBrightness);
    clk.setOnCCW(&decreaseBrightness);

    // _delay_ms(2000);

    while (1) {

        sw.process();
        clk.process();


        // sendPixel(255, 56, 1, 0);
        // sendPixel(100, 10, 1, 0);

        for (uint8_t i = 0; i < static_cast<uint8_t>(32); i++) {
            sendPixel(0, 0, 0, 0);
        }

        _delay_us(80); // sufficient
        // _delay_ms(1000);

        // for (uint8_t i = 0; i < (8*numRows); i++) {
        // I think the multiplication takes it over the edge

        // for (uint8_t i = 0; i < (8*4); i++) {
        for (auto i = 0; i < (8*numRows); i++) {
            // sendPixel(100, 10, 1, 0);
            // sendPixel(brightness, 10, 1, 0);
            sendPixel(brightness, 56, 1, 0);
            // sendPixel(brightness, 0, 0, 0);
        }

        // while (1) { }

        // sendPixel(255, 56, 1, 0);
        // sendPixel(255, 56, 1, 0);
        // sendPixel(255, 56, 1, 0);
        // sendPixel(255, 56, 1, 0);
        // sendPixel(255, 56, 1, 0);
        // sendPixel(255, 56, 1, 0);


        // if (abortTxP) {
        //     while (1) {
        //         _delay_us(80);
        //         sendPixel(0, 255, 0, 0);
        //     }
        // }

        if (numRows > 0) {
            LED::setLow();
        }
        else {
            LED::setHigh();
        }

        if (!sw.pendingDebounceTimeout() &&
            !clk.pendingDebounceTimeout()) {
            // HAL::Ticker::pause(); //  TODO  SOMETHING ABOUT THIS BREAKS LIGHT
            HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);
        }

    }
}

