#pragma once

#include <Arduino.h>

struct LEDController {
    LEDController(const uint8_t _pin) noexcept;
    void turnOn(bool)                 noexcept;
    private:
        uint8_t pin;
        bool isOn;
};


/*
struct LEDController {
  LEDController(const uint8_t _pin) : pin { _pin }, isOn { true } {
    pinMode(pin, OUTPUT);
  }

  void turnOn(bool on_p) {
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

  private:
    bool isOn;
    uint8_t pin;
};
*/
