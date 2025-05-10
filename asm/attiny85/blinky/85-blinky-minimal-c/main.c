
#include <stdint.h>

extern void start();
extern void blink(uint8_t);

int main() {
    start();
    while (1) {
        blink(1);
        blink(0);
    }
}
