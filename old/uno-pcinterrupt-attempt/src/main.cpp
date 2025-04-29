
#include <Arduino.h>

// LED and switch

constexpr uint16_t DEBOUNCE_WINDOW{ 1000 };
constexpr uint8_t LED_PIN           { 13 };
constexpr uint8_t BUTTON_PIN         { 7 };
constexpr uint16_t BAUD_RATE      { 9600 };

volatile bool buttonPressedP     { false };
volatile bool ledOnP             { false };        
volatile uint32_t now { 0 };
volatile bool lastButtonState    { HIGH };


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
    static uint32_t buttonLastPressed = 0;
    static uint32_t elapsed = 0;
    static bool withinWindow = false;

    now = millis();
    elapsed = now - buttonLastPressed;
    withinWindow = elapsed > DEBOUNCE_WINDOW;

    // Serial.print("interrupt received: ");
    // Serial.println(now);
    // Serial.print("button last pressed: ");
    // Serial.println(buttonLastPressed);
    // Serial.print("elapsed: ");
    // Serial.println(elapsed);

    if (elapsed > DEBOUNCE_WINDOW) {
        ledOnP = !ledOnP;
        digitalWrite(LED_PIN, ledOnP);
        buttonLastPressed = now;
        // Serial.println("pressed!");
        buttonPressedP = false;
    }
}

void loop() {
    if (buttonPressedP) handleButtonPress();
}

ISR (PCINT2_vect) {
    bool currentState = digitalRead(BUTTON_PIN);
    // is it falling?
    if (lastButtonState == HIGH && currentState == LOW) {
        buttonPressedP = true;
    }

    // Save for next edge check
    lastButtonState = currentState;
}



