#include <Arduino.h>

// Match JUCE MIDI CC assignments exactly
const int CC_STUTTER_SIZE = 14; // Pot 1: Stutter Size (0.0 to 1.0)
const int CC_WET_MIX      = 15; // Pot 2: Wet / Dry Mix (0.0 to 1.0)
const int CC_SPEED_PITCH  = 16; // Pot 3: Speed / Pitch (0.0 to 1.0)
const int CC_TRIGGER      = 17; // Button: Glitch Trigger (On/Off)

uint8_t currentBPM = 60;

void setup() {
  Serial.begin(115200);
}

void loop() {
  // Parse incoming binary packet from Python: [Sync Header 0xA5, BPM Value]
  if (Serial.available() >= 2) {
    if (Serial.read() == 0xA5) {
      currentBPM = Serial.read();
      dispatchToJuceEngine(currentBPM);
    }
  }

  // Flush internal USB MIDI buffer
  while (usbMIDI.read()) {}
}

void dispatchToJuceEngine(uint8_t bpm) {
  // Constrain BPM between resting (60) and panic (160)
  uint8_t clampedBPM = constrain(bpm, 60, 160);

  // 1. Wet/Dry Mix (CC 15): Higher BPM = wetter glitch mix
  uint8_t mixVal = map(clampedBPM, 60, 160, 20, 127);
  usbMIDI.sendControlChange(CC_WET_MIX, mixVal, 1);

  // 2. Stutter Size (CC 14): Higher BPM = tighter, faster stutter slices
  uint8_t sizeVal = map(clampedBPM, 60, 160, 127, 10);
  usbMIDI.sendControlChange(CC_STUTTER_SIZE, sizeVal, 1);

  // 3. Speed / Pitch (CC 16): Dynamic pitch shift scaling
  uint8_t speedVal = map(clampedBPM, 60, 160, 64, 127);
  usbMIDI.sendControlChange(CC_SPEED_PITCH, speedVal, 1);

  // 4. Glitch Trigger (CC 17): Enable trigger if heart rate exceeds 100 BPM
  uint8_t triggerVal = (bpm >= 100) ? 127 : 0;
  usbMIDI.sendControlChange(CC_TRIGGER, triggerVal, 1);

  // USB MIDI Flush
  usbMIDI.send_now();
}