#pragma once

#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>

/**
 * WATCHDOG 
 * 
 * The watchdog could be used as a dead-man's switch.
 * For example, if the main loop should take X ms, and you reset
 * the watchdog timer at the end of the loop, if there is ever any problem
 * with runaway code, the watchdog will timeout and reset the chip
 *
 * oscillates at 128 kHz
 * can be configured to interrupt instead of reset
 *
 * MCUSR - MCU status register
 *   - (1 << 3) - WDRF - Watchdog Reset Flag
 *     This bit is set if a Watchdog Reset occurs
 *
 *
 * WDTCR – Watchdog Timer Control Register
 *   - (1 << 7) - WDIF - Watchdog Timeout Interrupt Flag
 *
 *   - (1 << 6) - WDIE - Watchdog Timeout Interrupt Enable
 *     To avoid the Watchdog Reset, WDIE must be set after each interrupt
 *
 *   - (1 << 4) - WDCE - Watchdog Change Enable
 *   - (1 << 3) - WDE - Watchdog Enable
 *     Needs to be written to at the same time to initiate a timed sequence
 *     for either
 *       * disabling an enabled interrupt
 *       * changing the watchdog time-out (prescaler bits)
 *
 *       - the timeout 
 *       the configuration of the watchdog timer (or disabling it)
 *
 *   - Prescaler bits:
 *     (1 << 5) | (1 << 2) | (1 << 1) | (1 << 0)
 *     WDP3     | WDP2     | WDP1     | WDP0   num 128KHz cycles  ~ time (secs)
 *     0          0          0          0      2048               0.016
 *     0          0          0          1      4096               0.032
 *     0          0          1          0      8K                 0.064
 *     0          0          1          1      16K (2^14)         0.13
 *     0          1          0          0      32K                0.26
 *     0          1          0          1      64K                0.51
 *     0          1          1          0      128K               1.02
 *     0          1          1          1      256K               2.04
 *     1          0          0          0      512K               4.1
 *     1          0          0          1      1024K              8.2
 *
 */


namespace HAL {
namespace Watchdog {

static constexpr uint8_t get_wd_prescaler_bits(uint8_t pow_of_two) {
    /*
     * pow_of_two time (secs)
     * 11         0.016
     * 12         0.032
     * 13         0.064
     * 14         0.13
     * 15         0.26
     * 16         0.51
     * 17         1.02
     * 18         2.04
     * 19         4.1
     * 20         8.2
     */
    if      (pow_of_two == 11) return 0x00;
    else if (pow_of_two == 12) return (1 << WDP0);
    else if (pow_of_two == 13) return (1 << WDP1);
    else if (pow_of_two == 14) return (1 << WDP1) | (1 << WDP0);
    else if (pow_of_two == 15) return (1 << WDP2);
    else if (pow_of_two == 16) return (1 << WDP2) | (1 << WDP0);
    else if (pow_of_two == 17) return (1 << WDP2) | (1 << WDP1);
    else if (pow_of_two == 18) return (1 << WDP2) | (1 << WDP1) | (1 << WDP0);
    else if (pow_of_two == 19) return (1 << WDP3);
    else if (pow_of_two == 20) return (1 << WDP3) | (1 << WDP0);
    else                       return (1 << 0); // ???
}

template<uint8_t prescalerPowOf2>
struct Watchdog {
    static_assert(prescalerPowOf2 <= 20 && prescalerPowOf2 >= 11,
            "invalid prescaler value");
    static constexpr uint8_t prescaler_bits { 
        get_wd_prescaler_bits(prescalerPowOf2)
    };

    static inline void disable() {
        cli();
        MCUSR &= ~(1 << WDRF);
        WDTCR |= (1 << WDCE) | (1 << WDE);  // start timed sequence
        WDTCR = 0x00;                       // Disable WDT
        sei();
    }

    static inline void reset() {
        cli();
        MCUSR &= static_cast<uint8_t>(~(1 << WDRF));
        WDTCR |= (1 << WDCE) | (1 << WDE);
        WDTCR = (1 << WDIE) | prescaler_bits;
        sei();
    }
};

}
}
