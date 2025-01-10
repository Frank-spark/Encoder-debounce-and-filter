# Encoder Reading with Noise Mitigation

This project demonstrates how to use an ESP32 to read a quadrature encoder while applying filtering and debounce techniques to mitigate noise and ensure accurate position tracking. 

## Features
1. **Quadrature Decoder**: Tracks position and direction of rotation.
2. **Low-Pass Filtering**: Reduces high-frequency noise on the encoder signals.
3. **Debounce Mechanism**: Suppresses spurious transitions caused by noise or signal bounce.
4. **Customizable Parameters**: Easily adjust filtering and debounce settings for different encoders and environmental conditions.

---

## Code Explanation

### Setup
```cpp
const int encoderPinA = 2; // Pin for encoder channel A
const int encoderPinB = 3; // Pin for encoder channel B
```
- Define the GPIO pins connected to the encoder. Update these if your encoder is connected to different pins.

---

### Position Tracking
```cpp
volatile long encoderPosition = 0; // Variable to track encoder position
```
- Keeps track of the encoder's position. Use this variable to read the encoder's value.

---

### Filtering
```cpp
const float alpha = 0.1;           // Smoothing factor (0 < alpha < 1)
volatile float filteredStateA = 0; // Filtered signal for channel A
```
- **Purpose**: A simple low-pass filter smooths high-frequency noise.
- **Customization**:
  - Decrease `alpha` for more smoothing (better noise suppression but slower response).
  - Increase `alpha` for less smoothing (faster response but less noise suppression).

---

### Debouncing
```cpp
const unsigned long debounceInterval = 50; // Debounce time in microseconds
volatile unsigned long lastInterruptTime = 0;
```
- **Purpose**: Suppresses rapid transitions caused by noise or signal bounce.
- **Customization**:
  - Increase `debounceInterval` to improve noise immunity but risk missing rapid transitions.
  - Decrease `debounceInterval` for higher responsiveness but increased sensitivity to noise.

---

### Interrupt Service Routine (ISR)
```cpp
void IRAM_ATTR encoderISR() {
    unsigned long interruptTime = micros();

    // Check if the interrupt is within the debounce interval
    if (interruptTime - lastInterruptTime > debounceInterval) {
        // Read and filter signal, update position
        ...
    }

    // Update the last interrupt time
    lastInterruptTime = interruptTime;
}
```
- Reads and filters the encoder signal when a change is detected on channel A.
- Checks debounce conditions to prevent processing noisy or spurious signals.

---

### Main Loop
```cpp
void loop() {
    static long lastPosition = 0;

    // Read encoder position
    noInterrupts();
    long position = encoderPosition;
    interrupts();

    // Print position if it changes
    if (position != lastPosition) {
        Serial.println(position);
        lastPosition = position;
    }

    delay(10); // Reduce serial spam
}
```
- Periodically checks the encoder position and prints it if it has changed.

---

## Customization for Different Encoders
### Higher Resolution Encoders
- Use a **smaller debounce interval** (e.g., 10–50 µs) to detect rapid transitions.
- Test filtering settings to ensure smooth operation without missing transitions.

### Noisy Environments
- Increase `debounceInterval` (e.g., 100–500 µs) for better noise immunity.
- Decrease `alpha` (e.g., 0.05) for stronger noise suppression.

### Faster Encoders
- Ensure the debounce interval is shorter than the time between signal transitions at maximum speed.
- Reduce delays in the `loop()` function for faster position updates.

---

## How to Mitigate Interference
1. **Hardware Filtering**:
   - Add an RC filter (resistor-capacitor) to the encoder signal lines to reduce high-frequency noise.
   - Use shielded cables for the encoder to minimize electrical interference.

2. **Software Adjustments**:
   - Adjust `alpha` for stronger filtering.
   - Increase `debounceInterval` to reject spurious signals.

3. **Environmental Changes**:
   - Move the encoder wires away from high-current or high-frequency components.
   - Use differential signaling or optoisolators if the encoder supports them.

---

## Getting Started
1. Connect the encoder to the ESP32 as per the pin definitions.
2. Upload the code using the Arduino IDE.
3. Monitor the encoder position using the Serial Monitor.

---

## Future Improvements
- Add support for encoders with higher PPR or faster speeds.
- Implement more advanced filtering (e.g., band-pass or adaptive filters).
- Use external hardware filters for environments with significant electrical noise.

---

