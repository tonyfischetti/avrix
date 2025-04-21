#include <avr/io.h>
#include <util/delay.h>
#include "pin.h"

using LED    = Pin<0>;     // Pin 5
using Switch = Pin<1>;     // Pin 6

constexpr uint8_t analogPin = 0;

void adcInit() {
    ADMUX = (1 << MUX0); // Select ADC1 (PB2) or change to (0 << MUX0) for ADC0
    ADCSRA = (1 << ADEN)  // Enable ADC
           | (1 << ADPS1) | (1 << ADPS0); // Prescaler = 8 → ~1 MHz / 8 = 125 kHz
}

uint16_t analogRead() {
    ADCSRA |= (1 << ADSC);              // Start conversion
    while (ADCSRA & (1 << ADSC)) {}     // Wait until done
    return ADC;                         // Read result (10-bit)
}

void delay(uint16_t ms) {
    while (ms--) {
        _delay_ms(1);
    }
}

int main() {

    LED::setOutput();
    Switch::setInput();
    Switch::pullUp();

    adcInit();


    while (1) {
        if (Switch::read()) {
            LED::high();
        }
        else {
            uint16_t val = analogRead();           // 0–1023
            uint16_t ms = (val*val + 1); // NO      // map 0–1023 to 1–256 ms
            LED::high();
            delay(ms);
            LED::low();
            delay(ms);
        }
    }
}
