#include "SPI.h"

#define MCP4921_CS1_PIN  5  // Chip Select for DAC 1
#define MCP4921_CS2_PIN  4  // Chip Select for DAC 2

float voltage1 = 4.5;  // Initial voltage (no brake)
float voltage2 = 0.5;  // Initial voltage (no brake)
float targetVoltage1 = 4.5;  // Target voltage based on brake input
unsigned long lastInputTime = 0;  // Track last input time
const unsigned long inputTimeout = 200;  // 0.2-second timeout

void setup()
{ 
  Serial.begin(230400);
  //Serial.println("ESP32 MCP4921 DAC Control");

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
  // Check for serial commands
  if (Serial.available()) {
    // Check for single character commands first
    if (Serial.peek() == 'i') {
      Serial.read(); // Consume the 'i'
      Serial.println("brake");
    }
    else {
      // Process brake values
      String input = Serial.readStringUntil('\n');
      float brake = input.toFloat();
      if (brake >= 0.0 && brake <= 1.0) {
        targetVoltage1 = 4.5 - (brake * 4.0);
        lastInputTime = millis();
      }
    }
  }

  // If no input received for 0.2 seconds, return to no brake
  if (millis() - lastInputTime > inputTimeout) {
    targetVoltage1 = 4.5;  // Return to no brake position
    //Serial.println("No input for 0.2 seconds.");
  }

  // Smoothly transition voltages
  if (voltage1 > targetVoltage1) {
    voltage1 -= 0.02;
  } else if (voltage1 < targetVoltage1) {
    voltage1 += 0.02;
  }

  // Set DAC2 to always be the opposite of DAC1
  voltage2 = 5.0 - voltage1;

  // Apply new voltage values
  setDACVoltage(MCP4921_CS1_PIN, voltage1);
  setDACVoltage(MCP4921_CS2_PIN, voltage2);
} 