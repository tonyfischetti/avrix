
#include "LEDController.h"

LEDController::LEDController(const uint8_t _pin) noexcept
    : pin  { _pin },
      isOn { true } {
    pinMode(pin, OUTPUT);
}

void LEDController::turnOn(bool on_p) noexcept {
    if (on_p) {
        if (!isOn) {
            digitalWrite(pin, HIGH);
            isOn = true;
        }
    } else {
        if (isOn) {
            digitalWrite(pin, LOW);
            isOn = false;
        }
    }
}
