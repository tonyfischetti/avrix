#include <iostream>
#include <cstdlib>



uint8_t randomIntFromUpTo(uint8_t from, uint8_t to) {
    return (rand() % (to-from+1)) + from;
}

int main() {

    for (auto i = 0; i < 10000; i++) {
        printf("%d\n", randomIntFromUpTo(0, 1));
    }
}

