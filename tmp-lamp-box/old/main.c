#include <stdint.h>

#include <avr/io.h>
#include <avr/power.h>
#include <util/atomic.h>
#include <util/delay.h>

extern void start();
extern void send255();
extern void send007();
extern void send000();

#define NEO 1
#define LED 2

int main() {

    start();

    _delay_ms(500);

    while (1) {

        for (uint8_t i = 0; i < 4; i++) {
            send000();
            send000();
            send000();
            send000();
        }
        _delay_ms(1);

            send007();
            send000();
            send000();
            send000();

        /*     send000(); */
        /*     send255(); */
        /*     send000(); */
        /*     send000(); */
        /*  */
        /*     send000(); */
        /*     send000(); */
        /*     send255(); */
        /*     send000(); */

            /* send255(); */
            /* send255(); */
            /* send255(); */
            /* send000(); */


        while(1) {
            /* if (!SW::read() { */
            /*     for (uint8_t i = 0; i < 3; i++) { */
            /*         send000(); */
            /*         send000(); */
            /*         send000(); */
            /*         send225(); */
            /*     } */
            /* } */
        }

    }
}

