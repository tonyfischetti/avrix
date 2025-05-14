
# flickering LED candle

The goal for this one is to write flickering LED candle firmware for the
ATTiny85 microcontroller that uses as little power as possible.

It does it by spending most of its time in a deep sleep. It uses the
watchdog timer (on interrupt mode) to wake itself every ~125 ms. It also
uses a weird trick to randomize the behavior of the three flickering LEDs
only calling `rand()` once. No effort is made to set a random seed

The flickering LEDs are on pins 5, 6, and 7. RESET is pulled up (10K Ω).

The fuses are set such that it uses the internal oscillator divided by 8,
making the clock speed 1 MHz.

