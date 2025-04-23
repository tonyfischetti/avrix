
#include <Arduino.h>

// LED and switch

constexpr uint8_t DEBOUNCE_WINDOW  { 100 };
constexpr uint8_t LED_PIN           { 13 };
constexpr uint8_t BUTTON_PIN         { 7 };
constexpr uint16_t BAUD_RATE      { 9600 };

volatile bool buttonPressedP     { false };
volatile bool ledOnP             { false };        


void setup() {

    Serial.begin(BAUD_RATE);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledOnP);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Enable PCIE2 Bit3 = 1 (Port D)
    PCICR |= B00000100;
    // Select PCINT23 Bit7 = 1 (Pin D7)
    PCMSK2 |= B10000000;

}

void handleButtonPress() {
    buttonPressedP = false;
    ledOnP = !ledOnP;
    digitalWrite(LED_PIN, ledOnP);
    Serial.println("pressed");
}

void loop() {
    if (buttonPressedP) handleButtonPress();
}

ISR (PCINT2_vect) {
    buttonPressedP = !buttonPressedP;
}



