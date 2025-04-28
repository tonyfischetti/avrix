#include <avr/io.h>
#include <util/delay.h>

#define _F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR 103
// (_F_CPU/16/BAUD-1)

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


static char z[] = "Hello, world\r\n";

int main(void) {

    DDRB = 0b11111111;
    uart_init(MYUBRR);  // Initialize UART
    // _UART_init(BAUD);

    while (1) {
        PORTB = 0b11111111;
        _delay_ms(1000);
        PORTB = 0b00000000;
        _delay_ms(1000);
        uart_print(z);
        // uart_transmit(101);
    }
    return 0;
}
