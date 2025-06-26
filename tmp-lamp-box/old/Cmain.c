
#include <stdint.h>
#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

/* extern void send255(); */
/* extern void send0(); */

#define NEO 1
#define LED 2

int main() {
    DDRB |= NEO;
    DDRB |= LED;
    while (1) {
        PORTB |= LED;
        _delay_ms(500);
        PORTB = 0;
        _delay_ms(500);
    }
}
