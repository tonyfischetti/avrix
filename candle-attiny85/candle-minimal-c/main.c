
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>

extern void start();
extern void init_lfsr(uint8_t seed);
extern uint8_t get_next_byte(void);

int main() {
    start();
    init_lfsr(93);

    PORTB = 0;
    PORTB = (1 << 3);
    _delay_ms(500);
    PORTB = (1 << 1);
    _delay_ms(500);
    PORTB = (1 << 2);
    _delay_ms(500);
    PORTB = 0;
    _delay_ms(500);

    while (1) {
        uint8_t current_rand = get_next_byte();
        if ((current_rand & (0x03) << 0)) PORTB |= (1 << 3);
        else                              PORTB &= ~(1 << 3);
        if ((current_rand & (0x03) << 2)) PORTB |= (1 << 1); 
        else                              PORTB &= ~(1 << 1);
        if ((current_rand & (0x03) << 4) ||
           !(current_rand & (0x0f)))      PORTB |= (1 << 2);
        else                              PORTB &= ~(1 << 2);

        _delay_ms(125);
    }
}
