#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "avril.hpp"
#include "drivers/Button.hpp"
#include "drivers/RotaryEncoder.hpp"


extern "C" {
    void sendbyte(uint8_t b);
}

static   uint8_t numRows        {    0 };
static   uint8_t brightness     {  255 };
volatile uint8_t previousPINB	{ 0xFF };

using NEO = HAL::GPIO::GPIO<5>;
using LED = HAL::GPIO::GPIO<6>;
HAL::Drivers::Button<3, 30, 1000, HIGH, true>     sw;
HAL::Drivers::RotaryEncoder<7, 2, 0, HIGH, true> clk;



ISR(PCINT0_vect) {
    HAL::Ticker::resume(1);
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t current = PINB;
    uint8_t changed = current ^ previousPINB;
    previousPINB = current;

    sw.notifyInterruptOccurred(now, changed);
    clk.notifyInterruptOccurred(now, changed);
}




void addAnotherRow() {
    numRows = (numRows + 1) % 5;
}

void sendQuadruplet(uint8_t red,
                    uint8_t green,
                    uint8_t blue,
                    uint8_t warm) {
    cli();
    sendbyte(green);
    sendbyte(red);
    sendbyte(blue);
    sendbyte(warm);
    sei();
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

int main() {

    power_adc_disable();
    power_timer1_disable();

    LED::setOutput();
    NEO::setOutput();

    HAL::Ticker::setupMSTimer();
    sw.begin();
    clk.begin();
    sei();

    _delay_ms(500);

    sw.setOnRelease(&addAnotherRow);
    clk.setOnCW(&increaseBrightness);
    clk.setOnCCW(&decreaseBrightness);

    while (1) {

        sw.process();
        clk.process();

        _delay_ms(10);

        // for (uint8_t i = 0; i < 32; i++) {
        //     sendQuadruplet(0, 0, 0, 0);
        // }
        //
        // _delay_ms(10);
        //
        // // sendQuadruplet(0, 0, 0, brightness);
        // // sendQuadruplet(0, 0, 0, brightness);
        // // sendQuadruplet(0, 0, 0, brightness);

        // clear
        for (uint8_t i = 0; i < numRows; i++) {
            sendQuadruplet(0, 0, 0, 0);
            sendQuadruplet(0, 0, 0, 0);
            sendQuadruplet(0, 0, 0, 0);
            sendQuadruplet(0, 0, 0, 0);
            sendQuadruplet(0, 0, 0, 0);
            sendQuadruplet(0, 0, 0, 0);
            sendQuadruplet(0, 0, 0, 0);
            sendQuadruplet(0, 0, 0, 0);
        }

        _delay_ms(10);

        // for (uint8_t i = 0; i < (numRows); i++) {
        //     sendQuadruplet(brightness, brightness/5, 1, 0);
        //     sendQuadruplet(brightness, brightness/5, 1, 0);
        //     sendQuadruplet(brightness, brightness/5, 1, 0);
        //     sendQuadruplet(brightness, brightness/5, 1, 0);
        //     sendQuadruplet(brightness, brightness/5, 1, 0);
        //     sendQuadruplet(brightness, brightness/5, 1, 0);
        //     sendQuadruplet(brightness, brightness/5, 1, 0);
        //     sendQuadruplet(brightness, brightness/5, 1, 0);
        //     _delay_us(20);
        // }

        _delay_ms(100);

        if (numRows > 0) {
            LED::setLow();
        }
        else {
            LED::setHigh();
        }


        if (!sw.pendingDebounceTimeout() &&
            !clk.pendingDebounceTimeout()) {
            HAL::Ticker::pause();
            HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);
        }

    }
}

