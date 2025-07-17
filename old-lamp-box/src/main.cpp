#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "avril.hpp"
#include "devices/RotaryEncoderWithButton.hpp"


extern "C" {
    void send_byte(uint8_t b);
    void send_pixel(uint8_t red,
                    uint8_t green,
                    uint8_t blue,
                    uint8_t white);
    void flood_pixels(uint8_t red,
                      uint8_t green,
                      uint8_t blue,
                      uint8_t white,
                      uint8_t num_pixels);

}

using NEO = HAL::GPIO::GPIO<5>;
using LED = HAL::GPIO::GPIO<6>;

HAL::Devices::RotaryEncoderWithButton<3, 30, 1000, HIGH, true,
                                      7, 2, 0, HIGH, true> reWithBtn;

constexpr uint8_t  LATCH_TIME_US  {    60 };
constexpr uint8_t  NUM_PIXELS     {    32 };
constexpr uint8_t  TOTAL_ROWS     {     5 }; // 0 counts
volatile  uint8_t  previousPINB	  {  0xFF };
volatile  uint8_t  abortTxP	      { false };
static    uint8_t  numRows        {     1 };
static    uint8_t  brightness     {     5 };
static    uint16_t strobeInterval {   500 };

using PatternFunction = void (*)();
constexpr uint8_t         NUM_PATTERNS              { 5 };
static    uint8_t         currentPatternIndex       { 1 };
static    PatternFunction patternList[NUM_PATTERNS] {   };

#define CURRENT_PATTERN patternList[currentPatternIndex]



void nextPattern() {
    currentPatternIndex = (currentPatternIndex + 1) % NUM_PATTERNS;
    abortTxP = true;
}

void previousPattern() {
    if (currentPatternIndex == 0) {
        currentPatternIndex = NUM_PATTERNS - 1;
    }
    else {
        currentPatternIndex--;
    }
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

    reWithBtn.notifyInterruptOccurred(now, changed);
}


void checkPeripherals() {
    reWithBtn.process();
}


void clearPixels() {
    // for (uint8_t i = 0; i < NUM_PIXELS; i++) {
    //     send_pixel(0, 0, 0, 0);
    // }
    flood_pixels(0, 0, 0, 0, NUM_PIXELS);
    _delay_us(LATCH_TIME_US);
}


void testPattern1() {
    reWithBtn.setOnCW(&increaseBrightness);
    reWithBtn.setOnCCW(&decreaseBrightness);

    if (numRows == 0) {
        clearPixels();
        return;
    }
    uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };

    flood_pixels(255, 57, 0, 0, totalPixels);
    _delay_us(LATCH_TIME_US);
}


void warmColorPattern() {
    reWithBtn.setOnCW(&increaseBrightness);
    reWithBtn.setOnCCW(&decreaseBrightness);

    if (numRows == 0) {
        clearPixels();
        return;
    }
    uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };

    uint8_t greenChannel = (brightness >> 4) +
                           (brightness >> 4) +
                           (brightness >> 4) +
                           (brightness >> 6) +
                           (brightness >> 5);

    flood_pixels(brightness, greenChannel, 0, 0, totalPixels);
    _delay_us(LATCH_TIME_US);
}

void warmLightPattern() {
    reWithBtn.setOnCW(&increaseBrightness);
    reWithBtn.setOnCCW(&decreaseBrightness);

    if (numRows == 0) {
        clearPixels();
        return;
    }
    uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };

    flood_pixels(0, 0, 0, brightness, totalPixels);
    _delay_us(LATCH_TIME_US);
}

void warmStrobePattern() {
    reWithBtn.setOnCW(&increaseStrobeInterval);
    reWithBtn.setOnCCW(&decreaseStrobeInterval);

    // uint8_t onP { false };
    while (!abortTxP) {

        if (numRows == 0) {
            clearPixels();
            return;
        }
        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };

        if (abortTxP) return;
        checkPeripherals();

        flood_pixels(0, 0, 0, 255, totalPixels);
        _delay_us(LATCH_TIME_US);

        if (abortTxP) return;
        checkPeripherals();

        for (uint16_t i = 0; i < strobeInterval; i++) {
            if (abortTxP) return;
            checkPeripherals();
            _delay_ms(1);
        }

        flood_pixels(0, 0, 0, 0, totalPixels);
        _delay_us(LATCH_TIME_US);

        if (abortTxP) return;
        checkPeripherals();

        for (uint16_t i = 0; i < strobeInterval; i++) {
            if (abortTxP) return;
            checkPeripherals();
            _delay_ms(1);
        }

        if (abortTxP) return;
        checkPeripherals();

    }
}



void bisexualSwitchPattern() {
    reWithBtn.setOnCW(&increaseStrobeInterval);
    reWithBtn.setOnCCW(&decreaseStrobeInterval);

    while (!abortTxP) {

        if (numRows == 0) {
            clearPixels();
            return;
        }
        uint8_t totalPixels { static_cast<uint8_t>(numRows * 8) };

        if (abortTxP) return;
        checkPeripherals();

        flood_pixels(255, 0, 121, 0, totalPixels);
        _delay_us(LATCH_TIME_US);

        if (abortTxP) return;
        checkPeripherals();

        for (uint16_t i = 0; i < strobeInterval; i++) {
            if (abortTxP) return;
            checkPeripherals();
            _delay_ms(1);
        }
        if (abortTxP) return;


        flood_pixels(255, 0, 255, 0, totalPixels);
        _delay_us(LATCH_TIME_US);

        if (abortTxP) return;
        checkPeripherals();

        for (uint16_t i = 0; i < strobeInterval; i++) {
            if (abortTxP) return;
            checkPeripherals();
            _delay_ms(1);
        }
        if (abortTxP) return;
        checkPeripherals();


        flood_pixels(0, 0, 255, 0, totalPixels);
        _delay_us(LATCH_TIME_US);

        if (abortTxP) return;
        checkPeripherals();

        for (uint16_t i = 0; i < strobeInterval; i++) {
            if (abortTxP) return;
            checkPeripherals();
            _delay_ms(1);
        }

        if (abortTxP) return;
        checkPeripherals();

    }
}




int main() {

    power_adc_disable();
    power_timer1_disable();

    NEO::setOutput();
    LED::setOutput();

    HAL::Ticker::setupMSTimer();

    reWithBtn.begin();

    sei();

    patternList[0] = &testPattern1;
    patternList[1] = &warmColorPattern;
    patternList[2] = &warmLightPattern;
    patternList[3] = &warmStrobePattern;
    patternList[4] = &bisexualSwitchPattern;

    reWithBtn.setOnRelease(&addAnotherRow);
    reWithBtn.setOnLongPress(&nextPattern);
    reWithBtn.setOnPressedCW(&nextPattern);
    reWithBtn.setOnPressedCCW(&previousPattern);
    // clk.setOnCW(&increaseBrightness);
    // clk.setOnCCW(&decreaseBrightness);


    while (1) {

        abortTxP = false;

        checkPeripherals();

        if (numRows > 0) LED::setLow();
        else             LED::setHigh();

        clearPixels();

        CURRENT_PATTERN();

        if (!reWithBtn.pendingDebounceTimeout() &&
            !abortTxP) {
            HAL::Ticker::pause();
            HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);
        }

    }
}

