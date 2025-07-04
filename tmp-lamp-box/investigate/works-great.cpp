#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "avril.hpp"
#include "drivers/Button.hpp"
#include "drivers/RotaryEncoder.hpp"


extern "C" {
    void send_byte(uint8_t b);
    void send_pixel_4_args(uint8_t red,
                           uint8_t green,
                           uint8_t blue,
                           uint8_t white);
}

using NEO = HAL::GPIO::GPIO<5>;
using LED = HAL::GPIO::GPIO<6>;
HAL::Drivers::Button<3, 30, 1000, HIGH, true>     sw;
HAL::Drivers::RotaryEncoder<7, 2, 0, HIGH, true> clk;

constexpr uint8_t  LATCH_TIME_US  {    80 };
constexpr uint8_t  NUM_PIXELS     {    32 };
constexpr uint8_t  TOTAL_ROWS     {     5 }; // 0 counts
volatile  uint8_t  previousPINB	  {  0xFF };
volatile  uint8_t  abortTxP	      { false };
static    uint8_t  numRows        {     1 };
static    uint8_t  brightness     {     0 };
static    uint16_t strobeInterval {   500 };

using PatternFunction = void (*)();
constexpr uint8_t         NUM_PATTERNS              { 3 };
static    uint8_t         currentPatternIndex       { 0 };
static    PatternFunction patternList[NUM_PATTERNS] {   };

#define CURRENT_PATTERN patternList[currentPatternIndex]



void nextPattern() {
    currentPatternIndex = (currentPatternIndex + 1) % NUM_PATTERNS;
    abortTxP = true;
}

void addAnotherRow() {
    numRows = (numRows + 1) % TOTAL_ROWS;
}

//  TODO  improve dryness
void increaseStrobeInterval() {
    strobeInterval += 10;
}

void decreaseStrobeInterval() {
    strobeInterval -= 10;
}

void increaseBrightness() {
    if (brightness > 245)
        brightness = 255;
    else
        brightness += 10;
}

void decreaseBrightness() {
    if (brightness < 10)
        brightness = 0;
    else
        brightness -= 10;
}




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


void checkPeripherals() {
    sw.process();
    clk.process();
}

void sendPixel(uint8_t red,
               uint8_t green,
               uint8_t blue,
               uint8_t warm) {
    send_byte(green);
    send_byte(red);
    send_byte(blue);
    send_byte(warm);
}


void clearPixels() {
    for (uint8_t i = 0; i < NUM_PIXELS; i++) {
        send_pixel_4_args(0, 0, 0, 0);
        if (abortTxP) return;
    }
    _delay_us(LATCH_TIME_US);
}


void warmColorPattern() {
    clk.setOnCW(&increaseBrightness);
    clk.setOnCCW(&decreaseBrightness);

    for (auto i = 0; i < numRows; i++) {
        send_pixel_4_args(brightness, brightness >> 2, 1, 0);
        if (abortTxP) return;
        send_pixel_4_args(brightness, brightness >> 2, 1, 0);
        if (abortTxP) return;
        send_pixel_4_args(brightness, brightness >> 3, 2, 0);
        if (abortTxP) return;
        send_pixel_4_args(brightness, brightness >> 5, 2, 0);
        if (abortTxP) return;
        send_pixel_4_args(brightness, brightness >> 5, 2, 0);
        if (abortTxP) return;
        send_pixel_4_args(brightness, brightness >> 3, 2, 0);
        if (abortTxP) return;
        send_pixel_4_args(brightness, brightness >> 2, 1, 0);
        if (abortTxP) return;
        send_pixel_4_args(brightness, brightness >> 2, 1, 0);
        if (abortTxP) return;
    }
    _delay_us(LATCH_TIME_US);
}

void warmLightPattern() {
    clk.setOnCW(&increaseBrightness);
    clk.setOnCCW(&decreaseBrightness);

    uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };
    for (uint8_t i = 0; i < totalPixels; i++) {
        send_pixel_4_args(0, 0, 0, brightness);
        if (abortTxP) return;
    }
    _delay_us(LATCH_TIME_US);
}

void warmStrobePattern() {
    clk.setOnCW(&increaseStrobeInterval);
    clk.setOnCCW(&decreaseStrobeInterval);

    // uint8_t onP { false };
    while (!abortTxP) {

        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };

        checkPeripherals();
        if (abortTxP) return;

        for (uint8_t i = 0; i < totalPixels; i++) {
            send_pixel_4_args(0, 0, 0, 255);
            if (abortTxP) return;
        }
        checkPeripherals();
        if (abortTxP) return;
        _delay_us(LATCH_TIME_US);
        checkPeripherals();
        if (abortTxP) return;
        for (uint16_t i = 0; i < strobeInterval; i++) {
            _delay_ms(1);
            checkPeripherals();
            if (abortTxP) return;
        }

        for (uint8_t i = 0; i < totalPixels; i++) {
            send_pixel_4_args(0, 0, 0, 0);
            checkPeripherals();
            if (abortTxP) return;
        }
        checkPeripherals();
        if (abortTxP) return;
        _delay_us(LATCH_TIME_US);
        checkPeripherals();
        if (abortTxP) return;
        for (uint16_t i = 0; i < strobeInterval; i++) {
            _delay_ms(1);
            checkPeripherals();
            if (abortTxP) return;
        }
        checkPeripherals();
        if (abortTxP) return;

    }
}


void bisexualStrobePattern() {
    clk.setOnCW(&increaseStrobeInterval);
    clk.setOnCCW(&decreaseStrobeInterval);

    // uint8_t onP { false };
    while (1) {

        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };

        if (abortTxP) return;

        for (uint8_t i = 0; i < totalPixels; i++) {
            // sendPixel(0, 0, 0, brightness);
            sendPixel(255, 0, 121, 0);
            if (abortTxP) return;
        }
        _delay_us(LATCH_TIME_US);
        for (uint16_t i = 0; i < strobeInterval; i++) {
            _delay_ms(1);
            if (abortTxP) return;
        }
        if (abortTxP) return;


        for (uint8_t i = 0; i < totalPixels; i++) {
            sendPixel(0, 0, 0, 0);
            if (abortTxP) return;
        }
        _delay_us(LATCH_TIME_US);
        for (uint16_t i = 0; i < strobeInterval; i++) {
            _delay_ms(1);
            if (abortTxP) return;
        }
        if (abortTxP) return;


        for (uint8_t i = 0; i < totalPixels; i++) {
            // sendPixel(0, 0, 0, brightness);
            sendPixel(255, 0, 255, 0);
            if (abortTxP) return;
        }
        _delay_us(LATCH_TIME_US);
        for (uint16_t i = 0; i < strobeInterval; i++) {
            _delay_ms(1);
            if (abortTxP) return;
        }
        if (abortTxP) return;

        for (uint8_t i = 0; i < totalPixels; i++) {
            sendPixel(0, 0, 0, 0);
            if (abortTxP) return;
        }
        _delay_us(LATCH_TIME_US);
        for (uint16_t i = 0; i < strobeInterval; i++) {
            _delay_ms(1);
            if (abortTxP) return;
        }
        if (abortTxP) return;

        for (uint8_t i = 0; i < totalPixels; i++) {
            // sendPixel(0, 0, 0, brightness);
            sendPixel(0, 0, 255, 0);
            if (abortTxP) return;
        }
        _delay_us(LATCH_TIME_US);
        for (uint16_t i = 0; i < strobeInterval; i++) {
            _delay_ms(1);
            if (abortTxP) return;
        }
        if (abortTxP) return;

        for (uint8_t i = 0; i < totalPixels; i++) {
            sendPixel(0, 0, 0, 0);
            if (abortTxP) return;
        }
        _delay_us(LATCH_TIME_US);
        for (uint16_t i = 0; i < strobeInterval; i++) {
            _delay_ms(1);
            if (abortTxP) return;
        }
        if (abortTxP) return;

    }
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

    patternList[0] = &warmColorPattern;
    patternList[1] = &warmLightPattern;
    patternList[2] = &warmStrobePattern;
    // patternList[3] = &bisexualStrobePattern;

    sw.setOnRelease(&addAnotherRow);
    sw.setOnLongPress(&nextPattern);
    // clk.setOnCW(&increaseBrightness);
    // clk.setOnCCW(&decreaseBrightness);


    while (1) {

        abortTxP = false;

        checkPeripherals();

        if (numRows > 0) LED::setLow();
        else             LED::setHigh();

        clearPixels();

        CURRENT_PATTERN();

        if (!sw.pendingDebounceTimeout() &&
            !clk.pendingDebounceTimeout() &&
            !abortTxP) {
            HAL::Ticker::pause();
            HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);
        }

    }
}

