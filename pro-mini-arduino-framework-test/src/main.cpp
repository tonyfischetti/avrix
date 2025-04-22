
#include <Arduino.h>


constexpr uint16_t BAUD_RATE { 9600 };

constexpr uint8_t LED1 { 4 };

void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(LED1, OUTPUT);
    Serial.println("setup completed");
}

void loop() {

    digitalWrite(LED1, HIGH);
    delay(500);
    digitalWrite(LED1, LOW);
    delay(500);
}
