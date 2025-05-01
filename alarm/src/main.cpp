#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define _F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR 103
// (_F_CPU/16/BAUD-1)
//

constexpr uint8_t RE_SW { 4 }; // INT0
constexpr uint8_t RE_CLK { 5 }; // INT1
constexpr uint8_t RE_DAT { 6 };

constexpr uint8_t LED1 { 25 };
constexpr uint8_t LED2 { 26 };
constexpr uint8_t LED3 { 27 };


volatile bool button_pressed_p { false };


void uart_init(unsigned int ubrr) {
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

    // Enable transmitter
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
    
}

void uart_transmit(uint8_t data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1<<UDRE0))) {}
    
    // Put data into buffer, sends the data
    UDR0 = data;
}


void uart_print(char* str) {
    uint8_t i = 0;
    // while (str[i]) {
    while (str[i]) {
        uart_transmit(str[i]);
        i++;
    }
}


static char z[] = "button pressed!\r\n";

int main(void) {

    DDRB = 0b11111111;
    uart_init(MYUBRR);
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);

    EIMSK |= (1 << INT0);
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    sei();

    while (1) {
        if (button_pressed_p) {
            uart_print(z);
            button_pressed_p = false;
        }
    }
    return 0;
}


ISR(INT0_vect) {
    PORTB ^= (1 << PB1); // Toggle PB5 (Arduino pin 13)
    button_pressed_p = true;
}
