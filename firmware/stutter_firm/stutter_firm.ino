#include <Arduino.h>

// --- Pin Assignments ---
const int PIN_STUTTER_SIZE = A0;  // Analog: Stutter Length / Grain Size
const int PIN_MIX          = A1;  // Analog: Wet/Dry Mix
const int PIN_SPEED_PITCH  = A2;  // Analog: Playback Speed / Pitch
const int PIN_GLITCH_BTN   = 2;   // Digital: Instant Glitch Trigger Button

// --- MIDI CC Assignments ---
const uint8_t CC_STUTTER_SIZE = 14;
const uint8_t CC_MIX          = 15;
const uint8_t CC_SPEED_PITCH  = 16;
const uint8_t CC_GLITCH_BTN   = 17;

const uint8_t MIDI_CHANNEL    = 1;

// --- ADC Filtering & Jitter Prevention ---
struct AnalogControl {
    int pin;
    uint8_t ccNumber;
    float smoothedValue;
    int lastMidiValue;
};

AnalogControl controls[] = {
    { PIN_STUTTER_SIZE, CC_STUTTER_SIZE, 0.0f, -1 },
    { PIN_MIX,          CC_MIX,          0.0f, -1 },
    { PIN_SPEED_PITCH,  CC_SPEED_PITCH,  0.0f, -1 }
};

const int NUM_CONTROLS = sizeof(controls) / sizeof(controls[0]);

// Digital Button State
int lastBtnState = HIGH;

void setup() {
    analogReadResolution(10); // 0 - 1023 resolution
    pinMode(PIN_GLITCH_BTN, INPUT_PULLUP);

    // Initialize smoothed values
    for (int i = 0; i < NUM_CONTROLS; i++) {
        pinMode(controls[i].pin, INPUT);
        controls[i].smoothedValue = analogRead(controls[i].pin);
    }
}

void loop() {
    // 1. Process Analog Controls with EMA Filtering
    for (int i = 0; i < NUM_CONTROLS; i++) {
        int raw = analogRead(controls[i].pin);

        // Exponential Moving Average filter (alpha = 0.15 for low noise without latency)
        controls[i].smoothedValue = (0.15f * raw) + (0.85f * controls[i].smoothedValue);

        // Map to 7-bit MIDI range (0 - 127)
        int midiVal = map((int)controls[i].smoothedValue, 0, 1023, 0, 127);
        midiVal = constrain(midiVal, 0, 127);

        // Only send when the MIDI step changes to prevent bus flooding
        if (midiVal != controls[i].lastMidiValue) {
            usbMIDI.sendControlChange(controls[i].ccNumber, midiVal, MIDI_CHANNEL);
            controls[i].lastMidiValue = midiVal;
        }
    }

    // 2. Process Instant Glitch Button (Momentary Trigger)
    int btnState = digitalRead(PIN_GLITCH_BTN);
    if (btnState != lastBtnState) {
        // Send CC 127 on press (LOW with PULLUP), CC 0 on release (HIGH)
        uint8_t btnMidiVal = (btnState == LOW) ? 127 : 0;
        usbMIDI.sendControlChange(CC_GLITCH_BTN, btnMidiVal, MIDI_CHANNEL);
        lastBtnState = btnState;
        delay(5); // Debounce
    }

    // 3. Flush Incoming USB MIDI Buffers
    while (usbMIDI.read()) {}

    delay(2); // ~500Hz loop rate
}