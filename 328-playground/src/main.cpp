#include "../avr-hal/include/hal.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define _F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR 103
// (_F_CPU/16/BAUD-1)
//

volatile bool buttonPressedP  { false };
volatile uint32_t lastPressed { 0     };
volatile bool watchdogBarkedP { false };


using RE_SW   = HAL::GPIO::GPIO<4>;
using LED0    = HAL::GPIO::GPIO<15>;
using LED1    = HAL::GPIO::GPIO<25>;
using LED2    = HAL::GPIO::GPIO<24>;
using LED3    = HAL::GPIO::GPIO<23>;
using MD1     = HAL::GPIO::GPIO<12>;
using MD2     = HAL::GPIO::GPIO<13>;
using MD3     = HAL::GPIO::GPIO<26>;
using RE_CLK  = HAL::GPIO::GPIO<5>;
using RE_DATA = HAL::GPIO::GPIO<6>;
using Button  = HAL::GPIO::GPIO<14>;

// using WD   = HAL::Watchdog::Watchdog<19>;


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


ISR(PCINT2_vect) {
    buttonPressedP = true;
}

// ISR(WDT_vect) {
//     watchdogBarkedP = true;
// }



static char alice[] = "::alice glass:: HI!\r\n";
static char es[] = "exit setup\r\n";
static char yes[] = "p!\r\n";
static char no[] = "!p\r\n";
static char period[] = ".";
static char newline[] = "\n";


void start_sequence() {
    _delay_ms(250);
    LED1::setHigh();
    _delay_ms(250);
    LED2::setHigh();
    _delay_ms(250);
    LED3::setHigh();
    _delay_ms(250);
    LED1::setLow();
    LED2::setLow();
    LED3::setLow();
}


int main(void) {

    uart_print(alice);

    RE_SW::setInputPullup();
    RE_CLK::setInputPullup();
    RE_DATA::setInputPullup();
    LED0::setOutput();
    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    MD1::setInput(); // pulled-down
    MD2::setInput(); // pulled-down
    MD3::setInput(); // pulled-down

    uart_init(MYUBRR);

    RE_SW::enablePCINT();
    
    // WD::reset();
    
    HAL::Ticker::setupMSTimer();

    sei();

    start_sequence();

    uart_print(es);

    while (1) {

        if (buttonPressedP) {
            buttonPressedP = false;
            uint32_t now = HAL::Ticker::getNumTicks();
            if ((now - lastPressed) > 250) {
                lastPressed = now;
                uart_print(yes);
                LED0::toggle();
            }
        }

    }
    return 0;
}

