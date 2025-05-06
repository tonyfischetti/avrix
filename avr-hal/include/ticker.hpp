#pragma once

#include "hal_common.hpp"

#include <stdint.h>
#include <util/atomic.h>
#include <avr/io.h>
#include <avr/interrupt.h> 

namespace HAL {
namespace Ticker {

void setupMSTimer();
uint32_t getNumTicks();

}
}
