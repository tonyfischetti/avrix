#include <cstdio>
#include <stdlib.h>

const unsigned long int numTrials { 500000000 };

unsigned long int pin1TurnedOn { 0 };
unsigned long int pin2TurnedOn { 0 };
unsigned long int pin3TurnedOn { 0 };

uint8_t randomIntFromUpTo(uint8_t from, uint8_t to) {
    return (rand() % (to-from+1)) + from;
}

int main() {

    for (auto i = 0; i < numTrials; i++) t {
        if (randomIntFromUpTo(0, 3)) pin1TurnedOn++;
        if (randomIntFromUpTo(0, 3)) pin2TurnedOn++;
        if (randomIntFromUpTo(0, 3)) pin3TurnedOn++;
    }

    printf("1: %.2f\n", static_cast<double>(pin1TurnedOn)/numTrials*100);
    printf("1: %.2f\n", static_cast<double>(pin2TurnedOn)/numTrials*100);
    printf("1: %.2f\n", static_cast<double>(pin3TurnedOn)/numTrials*100);
}

// 1: 66.67
// 1: 66.66
// 1: 66.67
// O0: User-mode: 13.32s       Kernel-mode: 0.02s      Wall: 13.47s    Perc: 99%
// 02: User-mode: 12.23s       Kernel-mode: 0.02s      Wall: 12.38s    Perc: 98%

// 1: 75.00
// 1: 75.00
// 1: 75.00
// O0: User-mode: 11.83s       Kernel-mode: 0.02s      Wall: 11.85s    Perc: 99%
// O2: User-mode: 10.68s       Kernel-mode: 0.01s      Wall: 11.06s    Perc: 96%
