#pragma once

#include <stdint.h>
#include <util/atomic.h>
#include <avr/io.h>

namespace HAL {
namespace Ticker {

void setupMSTimer();
uint32_t getNumTicks();

}
}
