#include <cstdio>
#include <stdlib.h>

const unsigned long int numTrials { 500000000 };
// const unsigned long int numTrials { 50000 };
// const unsigned long int numTrials { 50000 };

unsigned long int pin1TurnedOn { 0 };
unsigned long int pin2TurnedOn { 0 };
unsigned long int pin3TurnedOn { 0 };

uint8_t randomIntFromUpTo(uint8_t from, uint8_t to) {
    return (rand() % (to-from+1)) + from;
}


int main() {
    for (auto i = 0; i < numTrials; i++) {
        const uint8_t currentRand = randomIntFromUpTo(0, 63);

        if ((currentRand & (0x03) << 0)) pin1TurnedOn++;
        if ((currentRand & (0x03) << 2)) pin2TurnedOn++;
        if ((currentRand & (0x03) << 4)) pin3TurnedOn++;

    }

    printf("1: %.2f\n", static_cast<double>(pin1TurnedOn)/numTrials*100);
    printf("1: %.2f\n", static_cast<double>(pin2TurnedOn)/numTrials*100);
    printf("1: %.2f\n", static_cast<double>(pin3TurnedOn)/numTrials*100);
}


// 1: 75.00
// 1: 75.00
// 1: 75.00
// O0: User-mode: 6.33s        Kernel-mode: 0.01s      Wall: 6.48s     Perc: 97%
// O2: User-mode: 4.57s        Kernel-mode: 0.01s      Wall: 4.77s     Perc: 96%

