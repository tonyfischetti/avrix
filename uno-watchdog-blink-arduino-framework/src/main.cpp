
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <Arduino.h>


constexpr uint16_t BAUD_RATE { 9600 };

constexpr uint8_t LED1 { 4 };

volatile bool toggle_p { false };


ISR(WDT_vect) {
    toggle_p = true;
}


void reset_watchdog() {
    cli();
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);  // Enable timed changes
    WDTCSR = (1 << WDIE) | (1 << WDP3); // 2 seconds
    sei();
}

void toggle() {
    toggle_p = false;
    Serial.println("toggle triggered");
    delay(100);
}

void goToSleepSafely() {
    cli();
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sei();
    sleep_cpu();
    sleep_disable();
}


void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(LED1, OUTPUT);
    reset_watchdog();
    digitalWrite(LED1, HIGH);
}

void loop() {

    if (toggle_p) toggle();

    reset_watchdog();
    digitalWrite(LED1, LOW);
    goToSleepSafely();
    digitalWrite(LED1, HIGH);
}
