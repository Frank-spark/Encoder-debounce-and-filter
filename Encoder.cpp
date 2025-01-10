#include <Arduino.h>

// Define encoder pins
const int encoderPinA = 2; // Pin for encoder channel A
const int encoderPinB = 3; // Pin for encoder channel B

volatile long encoderPosition = 0; // Variable to track encoder position
volatile bool lastStateA = LOW;    // Last state of channel A

// Low-pass filter parameters
const float alpha = 0.1;           // Smoothing factor (0 < alpha < 1)
volatile float filteredStateA = 0; // Filtered signal for channel A

// Debounce parameters
const unsigned long debounceInterval = 50; // Debounce time in microseconds
volatile unsigned long lastInterruptTime = 0;

void IRAM_ATTR encoderISR() {
    unsigned long interruptTime = micros();
    
    // Check if the interrupt is within the debounce interval
    if (interruptTime - lastInterruptTime > debounceInterval) {
        // Read raw states of encoder channels
        bool currentStateA = digitalRead(encoderPinA);
        bool currentStateB = digitalRead(encoderPinB);

        // Apply low-pass filter to channel A
        filteredStateA = alpha * currentStateA + (1 - alpha) * filteredStateA;

        // Detect changes in the filtered signal
        if (filteredStateA > 0.5 && lastStateA <= 0.5) { // Rising edge
            if (currentStateB == LOW) {
                encoderPosition++;
            } else {
                encoderPosition--;
            }
        }

        lastStateA = filteredStateA > 0.5;
    }

    // Update the last interrupt time
    lastInterruptTime = interruptTime;
}

void setup() {
    pinMode(encoderPinA, INPUT_PULLUP);
    pinMode(encoderPinB, INPUT_PULLUP);

    // Attach interrupt to channel A
    attachInterrupt(digitalPinToInterrupt(encoderPinA), encoderISR, CHANGE);

    Serial.begin(115200);
}

void loop() {
    static long lastPosition = 0;

    // Read encoder position in the main loop
    noInterrupts();
    long position = encoderPosition;
    interrupts();

    // Only print if position has changed
    if (position != lastPosition) {
        Serial.println(position);
        lastPosition = position;
    }

    delay(10); // Small delay to reduce serial spam
}
