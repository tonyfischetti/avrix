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
HAL::Drivers::Button<3, 30, 1000, HIGH, true>     sw;
HAL::Drivers::RotaryEncoder<7, 2, 0, HIGH, true> clk;


volatile uint8_t previousPINB	{  0xFF };
volatile uint8_t abortTxP	    { false };
volatile uint8_t numPixels      {     5 };


void addAnotherPixel() {
    numPixels++;
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



void sendPixel(uint8_t red,
               uint8_t green,
               uint8_t blue,
               uint8_t warm) {
    sendbyte(green);
    sendbyte(red);
    sendbyte(blue);
    sendbyte(warm);
}

void clearPixel() {
    sendPixel(0, 0, 0, 0);
}

void flood(uint8_t red,
           uint8_t green,
           uint8_t blue,
           uint8_t warm) {
    _delay_us(100);
    for (uint8_t i = 0; i <= numPixels; i++) {
        sendPixel(red, green, blue, warm);
    }
    _delay_us(80);
}


int main() {

    power_adc_disable();
    power_timer1_disable();

    NEO::setOutput();
    HAL::Ticker::setupMSTimer();
    sw.begin();
    clk.begin();
    sei();


    sw.setOnRelease(&addAnotherPixel);

    _delay_ms(500);

    while (1) {

        sw.process();
        clk.process();

        _delay_us(300);

        sendPixel(255, 56, 1, 0);
        sendPixel(255, 56, 1, 0);
        sendPixel(255, 56, 1, 0);
        sendPixel(255, 56, 1, 0);
        sendPixel(255, 56, 1, 0);
        sendPixel(255, 56, 1, 0);
        sendPixel(255, 56, 1, 0);
        sendPixel(255, 56, 1, 0);


        // if (abortTxP) {
        //     while (1) {
        //         _delay_us(80);
        //         sendPixel(0, 255, 0, 0);
        //     }
        // }

    }
}

