#include <avr/io.h>
#include <util/delay.h>

int main() {
    // Set PB0 (pin 5) as output
    DDRB |= (1 << PB0);

    while (1) {
        // Toggle PB0
        PORTB ^= (1 << PB0);
        _delay_ms(500);
    }
}
