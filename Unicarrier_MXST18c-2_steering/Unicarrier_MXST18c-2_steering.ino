#include "SPI.h"

#define MCP4921_CS1_PIN  5  // Chip Select for DAC 1
#define MCP4921_CS2_PIN  4  // Chip Select for DAC 2

float voltage1 = 1.4;
float voltage2 = 3.4;
bool increasing1 = true;
bool increasing2 = true;
bool shouldUpdate = false;  // Controls if voltages should update
unsigned long lastToggleTime = 0;  // Track when to reverse steering

void setup()
{
  Serial.begin(115200);

  pinMode(MCP4921_CS1_PIN, OUTPUT);
  pinMode(MCP4921_CS2_PIN, OUTPUT);
  digitalWrite(MCP4921_CS1_PIN, HIGH);
  digitalWrite(MCP4921_CS2_PIN, HIGH);

  SPI.begin(18, -1, 23, -1);  // Initialize VSPI
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
  // Check for keyboard control via Serial
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    // Check for the 'i' character to identify device
    if (command == 'i') {
      Serial.println("steering");
    }
    else if (command == 'r') {  // Manual steering reversal
      increasing1 = !increasing1;
      increasing2 = !increasing2;
    } else if (command == 'p') {  // Key pressed -> Start voltage updates
      shouldUpdate = true;
    } else if (command == 's') {  // Key released -> Stop voltage updates
      shouldUpdate = false;
    }
  }

  // Only update voltages if a key is being held
  if (shouldUpdate) {
    // Update DAC1
    if (increasing1) {
      voltage1 += 0.02;
      if (voltage1 >= 4.45) increasing1 = false;
    } else {
      voltage1 -= 0.02;
      if (voltage1 <= 0.60) increasing1 = true;
    }

    // Update DAC2
    if (increasing2) {
      voltage2 += 0.02;
      if (voltage2 >= 4.45) increasing2 = false;
    } else {
      voltage2 -= 0.02;
      if (voltage2 <= 0.60) increasing2 = true;
    }
  }

  // Apply voltages to DACs
  setDACVoltage(MCP4921_CS1_PIN, voltage1);
  setDACVoltage(MCP4921_CS2_PIN, voltage2);
  delay(3);  // Small delay to prevent voltage settling
}