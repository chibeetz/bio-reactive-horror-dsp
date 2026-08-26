#include <SPI.h>

const int CS_PIN = 10;
const uint8_t PACKET_HEADER = 0xA5;

void setup() {
  Serial.begin(115200);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH); // De-assert CS
  
  SPI.begin();
  Serial.println("Teensy 4.1 SPI Boss Initialized.");
}

void loop() {
  // Transmit test 4-byte packet: Header 0xA5, Register 0x01, Value 1000 (0x03E8)
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  digitalWrite(CS_PIN, LOW); // Assert CS
  
  SPI.transfer(PACKET_HEADER);
  SPI.transfer(0x01); // Target Register (Audio Stutter Loop)
  SPI.transfer(0x03); // MSB
  SPI.transfer(0xE8); // LSB
  
  digitalWrite(CS_PIN, HIGH); // De-assert CS
  SPI.endTransaction();

  Serial.println("SPI Packet Sent: [0xA5, Reg: 0x01, MSB: 0x03, LSB: 0xE8]");
  delay(1000); // Pulse once per second
}