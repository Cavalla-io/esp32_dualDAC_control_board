#include "SPI.h"

#define MCP4921_CS1_PIN  5  // Chip Select for DAC 1
#define MCP4921_CS2_PIN  4  // Chip Select for DAC 2

float voltage1 = 1.4;
float voltage2 = 3.4;
bool increasing1 = true;
bool increasing2 = true;

void setup()
{
  Serial.begin(230400);
  Serial.println("ESP32 MCP4921 Dual DAC Control");

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
  if (voltage < 0.0 || voltage > 5.0) return;  // Ensure valid range
  uint16_t value = (voltage / 5.0) * 4095;
  uint16_t data = 0x3000 | value;
  
  digitalWrite(csPin, LOW);
  SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE0));  // 5 MHz SPI
  SPI.transfer((uint8_t)(data >> 8));
  SPI.transfer((uint8_t)(data & 0xFF));
  SPI.endTransaction();
  digitalWrite(csPin, HIGH);
}

void loop()
{
  Serial.print("DAC1 Voltage: ");
  Serial.print(voltage1);
  Serial.print("V, DAC2 Voltage: ");
  Serial.print(voltage2);
  Serial.println("V");

  // Update DAC1 independently
  if (increasing1) {
    voltage1 += 0.02;
    if (voltage1 >= 4.45) {
      increasing1 = false;
    }
  } else {
    voltage1 -= 0.02;
    if (voltage1 <= 0.55) {
      increasing1 = true;
    }
  }

  // Update DAC2 independently
  if (increasing2) {
    voltage2 += 0.02;
    if (voltage2 >= 4.45) {
      increasing2 = false;
    }
  } else {
    voltage2 -= 0.02;
    if (voltage2 <= 0.55) {
      increasing2 = true;
    }
  }

  setDACVoltage(MCP4921_CS1_PIN, voltage1);
  setDACVoltage(MCP4921_CS2_PIN, voltage2);
}
