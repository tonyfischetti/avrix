#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "avril.hpp"
#include "patterns/patterns.hpp"
#include "devices/RotaryEncoderWithButton.hpp"


using NEO = HAL::GPIO::GPIO<5>;
using LED = HAL::GPIO::GPIO<6>;

HAL::Devices::RotaryEncoderWithButton<3, 30, 1000, HIGH, true,
                                      7, 2, 0, HIGH, true> reWithBtn;

constexpr uint8_t TOTAL_ROWS                {                  5 }; // 0 counts
constexpr uint8_t NUM_PIXELS                { (TOTAL_ROWS-1) * 8 };
constexpr uint8_t NUM_CHANNELS              {     NUM_PIXELS * 4 };
volatile  uint8_t previousPINB              {               0xFF };
volatile  uint8_t abortTxP 	                {              false };
static    uint8_t pixelArray [NUM_CHANNELS] {                    };

static NCC_1701Pattern                   ncc1701Pattern        { pixelArray };
static CandlePattern                     candlePattern         { NUM_CHANNELS, pixelArray };
static WarmLightPattern     <TOTAL_ROWS> warmLightPattern      {};
static WarmColorPattern2    <TOTAL_ROWS> warmColorPattern2     {};
static WarmColorPattern3    <TOTAL_ROWS> warmColorPattern3     {};
static BisexualSwitchPattern<TOTAL_ROWS> bisexualSwitchPattern {};
static WarmStrobePattern    <TOTAL_ROWS> warmStrobePattern     {};
static SpectrumPattern      <TOTAL_ROWS> spectrumPattern       {};
static ChooseAColorPattern  <TOTAL_ROWS> chooseAColorPattern   {};
// static WarmColorPattern     <TOTAL_ROWS> warmColorPattern      {};
// static CoolWhiteColorPattern<TOTAL_ROWS> coolWhiteColorPattern {};

static Pattern* patternList[] = {
    &ncc1701Pattern,
    &candlePattern,
    &warmLightPattern,
    // &warmColorPattern,
    &warmColorPattern2,
    &warmColorPattern3,
    // &coolWhiteColorPattern,
    &spectrumPattern,
    &chooseAColorPattern,
    &bisexualSwitchPattern,
    &warmStrobePattern
};

constexpr uint8_t NUM_PATTERNS = sizeof(patternList) / sizeof(patternList[0]);
static    uint8_t  currentPatternIndex       { 0 };

#define CURRENT_PATTERN patternList[currentPatternIndex]


ISR(PCINT0_vect) {
    abortTxP = true;
    HAL::Ticker::resume(1);
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t current = PINB;
    uint8_t changed = current ^ previousPINB;
    previousPINB = current;

    reWithBtn.notifyInterruptOccurred(now, changed);
    reWithBtn.process();
}

inline void checkPeripherals() {
    reWithBtn.process();
}

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

int main() {

    power_adc_disable();
    power_timer1_disable();

    NEO::setOutput();
    LED::setOutput();

    HAL::Ticker::setupMSTimer();

    reWithBtn.begin();

    sei();

    reWithBtn.setOnLongPress(&nextPattern); //  TODO  ???
    reWithBtn.setOnPressedCW(&nextPattern);
    reWithBtn.setOnPressedCCW(&previousPattern);

    reWithBtn.setOnCW([]()      { CURRENT_PATTERN->onCW();      });
    reWithBtn.setOnCCW([]()     { CURRENT_PATTERN->onCCW();     });
    reWithBtn.setOnRelease([]() { CURRENT_PATTERN->onRelease(); });


    while (1) {

        // pattern potentially changed
        if (abortTxP) {
            flood_pixels(0, 0, 0, 0, TOTAL_ROWS*8);
            latch();
            abortTxP = false;
        }

        checkPeripherals();

        uint16_t delayTimeP = CURRENT_PATTERN->tick();
        latch();

        if (delayTimeP) {
            for (uint16_t i = 0; i < delayTimeP; i++) {
                if (abortTxP) break; //  TODO  ??
                checkPeripherals();
                _delay_ms(1);
            }
        }  //  TODO  use a timer instead
        else {
            if (!reWithBtn.pendingDebounceTimeout() &&
                !abortTxP) {
                HAL::Ticker::pause();
                HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);
            }
        }

    }
}

