#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>

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

    // no process() here: the input state machines (and their callbacks)
    // run only in main-loop context, so their state is never mutated
    // from two contexts at once. checkPeripherals() picks this up
    // within a millisecond.
    reWithBtn.notifyInterruptOccurred(now, changed);
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
            // clear first: an interrupt arriving during the flood
            // re-raises the flag and gets its own blank next lap,
            // instead of having its request erased
            abortTxP = false;
            flood_pixels(0, 0, 0, 0, TOTAL_ROWS*8);
            latch();
        }

        checkPeripherals();

        uint16_t delayTimeP = CURRENT_PATTERN->tick();
        latch();

        if (delayTimeP) {
            // pace the frame in IDLE sleep instead of a _delay_ms spin:
            // Timer0 keeps running in IDLE, so its 1 ms tick (or any pin
            // change) wakes us. An interrupt landing between the check
            // and sleep_cpu() costs at most one tick of oversleep, so
            // no atomic gate is needed here (unlike the PWR_DOWN path)
            uint32_t frameStart = HAL::Ticker::getNumTicks();
            while (!abortTxP &&
                   (HAL::Ticker::getNumTicks() - frameStart) < delayTimeP) {
                checkPeripherals();
                HAL::Sleep::goToSleep(SLEEP_MODE_IDLE);
            }
        }
        else {
            // decide-to-sleep must be atomic: an interrupt between the
            // check and sleep_cpu() would be processed and then slept
            // through, leaving its effects unrendered until the next
            // wake-up. goToSleep()'s sei();sleep_cpu() pair guarantees
            // a pending wake source aborts the sleep immediately.
            cli();
            if (!reWithBtn.pendingDebounceTimeout() &&
                !abortTxP) {
                HAL::Ticker::pause();
                HAL::Sleep::goToSleep(SLEEP_MODE_PWR_DOWN);
            }
            else {
                sei();
            }
        }

    }
}

