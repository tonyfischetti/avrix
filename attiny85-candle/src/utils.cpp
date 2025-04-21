#include "utils.h"

uint8_t randomIntFromUpTo(uint8_t from, uint8_t to) {
    return (rand() % (to-from+1)) + from;
}

