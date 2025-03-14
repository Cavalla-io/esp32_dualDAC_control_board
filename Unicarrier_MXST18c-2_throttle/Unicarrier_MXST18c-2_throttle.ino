#include "SPI.h"

#define MCP4921_CS1_PIN  5  // Chip Select for DAC 1
#define MCP4921_CS2_PIN  4  // Chip Select for DAC 2

float voltage1 = 4.5;
float voltage2 = 0.5;
bool shouldMoveToNeutral = false;  // Track if we are transitioning to 2.5V
unsigned long lastInputTime = 0;  // Track last input time
const unsigned long inputTimeout = 200;  // 2-second timeout

void setup()
{ 
  Serial.begin(230400);
  Serial.println("ESP32 MCP4921 DAC Control");

  pinMode(MCP4921_CS1_PIN, OUTPUT);
  pinMode(MCP4921_CS2_PIN, OUTPUT);
  digitalWrite(MCP4921_CS1_PIN, HIGH);
  digitalWrite(MCP4921_CS2_PIN, HIGH);

  SPI.begin(18, -1, 23, -1);  // Initialize VSPI

  // Set initial DAC values
  setDACVoltage(MCP4921_CS1_PIN, voltage1);
  setDACVoltage(MCP4921_CS2_PIN, voltage2);
}

void setDACVoltage(uint8_t csPin, float voltage)
{
  if (voltage < 0.0) voltage = 0.0;
  if (voltage > 5.0) voltage = 5.0;  // Ensuring the correct range

  uint16_t value = (voltage / 5.0) * 4095;
  uint16_t data = 0x3000 | value;

  digitalWrite(csPin, LOW);
  SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE0));
  SPI.transfer((uint8_t)(data >> 8));
  SPI.transfer((uint8_t)(data & 0xFF));
  SPI.endTransaction();
  digitalWrite(csPin, HIGH);
}

void loop()
{
  // Check for serial commands (keyboard control)
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == 'p') {  // Key `W` pressed -> Move both DACs to 2.5V
      shouldMoveToNeutral = true;
      lastInputTime = millis();  // Reset timeout
    } else if (command == 's') {  // Key released -> Return to default state
      shouldMoveToNeutral = false;
      lastInputTime = millis();  // Reset timeout
    }

    lastInputTime = millis();  // Update last input time
  }

  // If no input received for 0.2 seconds, stop moving
  if (millis() - lastInputTime > inputTimeout) {
    shouldMoveToNeutral = false;
    Serial.println("No input for 0.2 seconds.");
    //lastInputTime = millis();
  }

  // Smoothly transition voltages
  if (shouldMoveToNeutral) {
    if (voltage1 > 2.5) voltage1 -= 0.02;
    if (voltage1 < 2.5) voltage1 += 0.02;
  } else {
    if (voltage1 < 4.5) voltage1 += 0.02;
    if (voltage1 > 4.5) voltage1 -= 0.02;
  }

  // Set DAC2 to always be the opposite of DAC1
  voltage2 = 5.0 - voltage1;

  // Apply new voltage values
  setDACVoltage(MCP4921_CS1_PIN, voltage1);
  setDACVoltage(MCP4921_CS2_PIN, voltage2);
}
