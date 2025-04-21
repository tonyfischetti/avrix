#include <Arduino.h>
#include "led.h"

void initLED() {
  pinMode(13, OUTPUT);
}

void toggleLED() {
  digitalWrite(13, !digitalRead(13));
}
