
#include <Arduino.h>
#include "LEDController.h"

constexpr uint16_t BAUD_RATE { 9600 };

constexpr uint8_t LED1 { 10 };
constexpr uint8_t LED2 { 12 };
constexpr uint8_t LED3 { 11 };

uint16_t       cycleLength { 1023 };
uint16_t       counter     { 0    };
LEDController  led1        { LED1 };
LEDController  led2        { LED2 };
LEDController  led3        { LED3 };


void setup() {
}

void loop() {
  // cycleLength = analogRead(POT) * 3;
  cycleLength = 1023;

  counter = millis() % cycleLength;

  if (counter < ((cycleLength/2) + (random(0, 1000)-500))) {
    led1.turnOn(true);
  } else {
    led1.turnOn(false);
  }

  // if (counter < cycleLength / random(3, 20)) {
  if (counter < (cycleLength / random(3, 20))) {
    led2.turnOn(false);
  } else {
    led2.turnOn(true);
  }

  // if (counter < cycleLength / random(3, 20)) {
  if (counter < (cycleLength / random(3, 20))) {
    led3.turnOn(true);
  } else {
    led3.turnOn(false);
  }

}
