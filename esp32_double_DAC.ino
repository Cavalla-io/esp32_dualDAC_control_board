//
//    FILE: MCP4921_dual_memorysafe.ino
//  AUTHOR: Your Name
// PURPOSE: Memory-safe control of two MCP4921 DACs
//     URL: https://github.com/RobTillaart/MCP_DAC

#include "SPI.h"

#define MCP4921_CS1_PIN  5  // Chip Select for DAC 1
#define MCP4921_CS2_PIN  4  // Chip Select for DAC 2

void setup()
{
  Serial.begin(9600);
  Serial.println("ESP32 MCP4921 Dual DAC Control");

  pinMode(MCP4921_CS1_PIN, OUTPUT);
  pinMode(MCP4921_CS2_PIN, OUTPUT);
  digitalWrite(MCP4921_CS1_PIN, HIGH);
  digitalWrite(MCP4921_CS2_PIN, HIGH);

  SPI.begin(18, -1, 23, -1);  // Initialize VSPI

  Serial.println("Enter values: D1=2.5,D2=3.3");
}

void mcp4921(uint8_t csPin, uint16_t value)
{
  uint16_t data = 0x3000 | value;
  digitalWrite(csPin, LOW);

  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));  // 1 MHz SPI
  SPI.transfer((uint8_t)(data >> 8));
  SPI.transfer((uint8_t)(data & 0xFF));
  SPI.endTransaction();

  digitalWrite(csPin, HIGH);
}

void loop()
{
  char inputBuffer[32];  // Fixed-size buffer to prevent memory fragmentation
  float voltage1 = -1, voltage2 = -1;

  if (Serial.available() > 0) {
    int bytesRead = Serial.readBytesUntil('\n', inputBuffer, sizeof(inputBuffer) - 1);
    inputBuffer[bytesRead] = '\0';  // Ensure null termination

    // Parse input safely
    char *ptr = strtok(inputBuffer, ",");
    while (ptr != NULL) {
      if (strncmp(ptr, "D1=", 3) == 0) voltage1 = atof(ptr + 3);
      if (strncmp(ptr, "D2=", 3) == 0) voltage2 = atof(ptr + 3);
      ptr = strtok(NULL, ",");
    }

    // Validate & set DAC1
    if (voltage1 >= 0.0 && voltage1 <= 5.0) {
      uint16_t dacValue1 = (voltage1 / 5.0) * 4095;
      mcp4921(MCP4921_CS1_PIN, dacValue1);
      Serial.print("DAC1 Set: ");
      Serial.print(voltage1);
      Serial.println("V");
    } else if (voltage1 != -1) {
      Serial.println("Invalid voltage for DAC1! Enter 0-5V.");
    }

    // Validate & set DAC2
    if (voltage2 >= 0.0 && voltage2 <= 5.0) {
      uint16_t dacValue2 = (voltage2 / 5.0) * 4095;
      mcp4921(MCP4921_CS2_PIN, dacValue2);
      Serial.print("DAC2 Set: ");
      Serial.print(voltage2);
      Serial.println("V");
    } else if (voltage2 != -1) {
      Serial.println("Invalid voltage for DAC2! Enter 0-5V.");
    }

    while (Serial.available()) Serial.read();  // **Flush Serial buffer**
  }
}
