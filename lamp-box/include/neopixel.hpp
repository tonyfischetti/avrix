#pragma once

#include <stdint.h>
#include <util/delay.h>

constexpr uint8_t LATCH_TIME_US { 60 };

#ifdef __cplusplus
extern "C" {
#endif

    void send_byte(uint8_t b);
    void send_pixel(uint8_t red,
                    uint8_t green,
                    uint8_t blue,
                    uint8_t white);
    void flood_pixels(uint8_t red,
                      uint8_t green,
                      uint8_t blue,
                      uint8_t white,
                      uint8_t num_pixels);

#ifdef __cplusplus
}
#endif

inline void latch() {
    _delay_us(LATCH_TIME_US);
}
