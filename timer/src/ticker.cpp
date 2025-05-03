
#include <stdint.h>
#include <util/atomic.h>
#include <avr/io.h>

namespace HAL {
namespace Ticker {
    volatile uint32_t ticks = 0;

    void setup_ms_timer() {
        TCCR0A = (1 << WGM01);      // CTC mode
        TCCR0B = (1 << CS01);
        OCR0A = 124;
        TIMSK |= (1 << OCIE0A);     // Enable compare match A interrupt
    }

    uint32_t get_ticks() {
        uint32_t value;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        value = ticks;
        }
        return value;
    }
}
}

ISR(TIM0_COMPA_vect) {
    HAL::Ticker::ticks++;
}
