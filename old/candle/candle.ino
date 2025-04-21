#include <Arduino.h>
#include <SoftwareSerial.h>


constexpr uint16_t BAUD_RATE { 9600 };

constexpr uint8_t RX   { 3 };  // pin 2
constexpr uint8_t TX   { 4 };  // pin 3
// constexpr uint8_t POT  { 1 };  // pin 7
constexpr uint8_t LED1 { 0 };  // pin 5
constexpr uint8_t LED2 { 1 };  // pin 7


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


SoftwareSerial mySerial    { RX, TX };
uint16_t       cycleLength { 1023 };
uint16_t       counter     { 0    };
LEDController  led1        { LED1 };
LEDController  led2        { LED2 };




void setup() {
  mySerial.begin(BAUD_RATE);
}

void loop() {

  // cycleLength = analogRead(POT) * 3;
  cycleLength = 1023;

  mySerial.println(cycleLength);
  counter = millis() % cycleLength;
  mySerial.println(counter);
  mySerial.println("------");

  if (counter < cycleLength/2) {
    mySerial.println("turning on");
    led1.turnOn(true);
  } else {
    mySerial.println("turning off");
    led1.turnOn(false);
  }

  if (counter < cycleLength/10) {
    led2.turnOn(false);
  } else {
    led2.turnOn(true);
  }

}
