
#include <stdint.h>
#include <util/atomic.h>
#include <avr/io.h>
#include <avr/interrupt.h> 

namespace HAL {
namespace Ticker {

volatile uint32_t ticks = 0;

//  TODO  assumes 16MHz. fix it!
//  HACK  assumes 16MHz

void setupMSTimer() {
    TCCR0A = (1 << WGM01);      // CTC mode
    TCCR0B = (1 << CS01);
    TCCR0B = (1 << CS01) | (1 << CS00); // 64
    OCR0A  = 249;
    TIMSK0 |= (1 << OCIE0A);
}

uint32_t getNumTicks() {
    uint32_t value;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        value = ticks;
    }
    return value;
}

}
}

ISR(TIMER0_COMPA_vect) {
    HAL::Ticker::ticks++;
}
