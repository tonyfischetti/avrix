#include "avr-hal.hpp"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "PCINTDebouncer.hpp"

#define _F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR 103
// (_F_CPU/16/BAUD-1)
//

volatile uint32_t lastPressed { 0     };
volatile bool watchdogBarkedP { false };
volatile uint8_t previousPIND { 0xFF  };

volatile bool buttonPressedP  { false };
volatile bool reChangedP { false };


using LED0    = HAL::GPIO::GPIO<15>;
using LED1    = HAL::GPIO::GPIO<25>;
using LED2    = HAL::GPIO::GPIO<24>;
using LED3    = HAL::GPIO::GPIO<23>;
using MD1     = HAL::GPIO::GPIO<12>;
using MD2     = HAL::GPIO::GPIO<13>;
using MD3     = HAL::GPIO::GPIO<26>;
// using RE_CLK  = HAL::GPIO::GPIO<5>;
using RE_DATA = HAL::GPIO::GPIO<6>;
using Button  = HAL::GPIO::GPIO<14>;

// using WD   = HAL::Watchdog::Watchdog<19>;

HAL::GPIO::GPIO<4> RE_SW;
PCINTDebouncer RE_SW_D(true, 20);

HAL::GPIO::GPIO<5> RE_CLK;
PCINTDebouncer RE_CLK_D(true, 20);


ISR(PCINT2_vect) {
    uint32_t now = HAL::Ticker::getNumTicks();
    uint8_t current = PINB;
    uint8_t changed = current ^ previousPIND;
    previousPIND = current;

    if (changed & (1 << 2)) {
        buttonPressedP = RE_SW_D.registerInterrupt(now, (current & (1 << 2)) > 0);
    }
    if (changed & (1 << 3)) {
        reChangedP = RE_CLK_D.registerInterrupt(now, (current & (1 << 3)) > 0);
    }
}


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

    RE_SW.setInputPullup();
    RE_CLK.setInputPullup();
    RE_DATA::setInputPullup();
    LED0::setOutput();
    LED1::setOutput();
    LED2::setOutput();
    LED3::setOutput();
    MD1::setInput(); // pulled-down
    MD2::setInput(); // pulled-down
    MD3::setInput(); // pulled-down

    uart_init(MYUBRR);

    RE_SW.enablePCINT();
    RE_CLK.enablePCINT();
    
    // WD::reset();
    
    HAL::Ticker::setupMSTimer();

    sei();

    start_sequence();

    uart_print(es);

    while (1) {

        if (buttonPressedP) {
            buttonPressedP = false;
            if (!RE_SW_D.status()) {
                LED0::toggle();
            }
        }
        if (reChangedP) {
            reChangedP = false;
            if (!RE_CLK_D.status()) {
                LED3::toggle();
            }
        }

    }
    return 0;
}

