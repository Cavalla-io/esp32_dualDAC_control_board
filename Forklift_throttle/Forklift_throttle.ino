#include <SPI.h>

const int CS_PIN = 10;      // Chip Select pin for the MCP4921
const float minVoltage = 0;  // Minimum voltage
const float maxVoltage = 5.0;  // Maximum voltage
int dacValue;

void setup() {
  SPI.begin();
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  // Ensure CS is initially high
  Serial.begin(115200);

  // Give some time for the system to stabilize
  delay(100);

  while (Serial.available() > 0) {
    Serial.read();
  }
}

void loop() {
  if (Serial.available() > 0) {

    // Read the input voltage as a float
    float desiredVoltage;
    Serial.readBytes((char*)&desiredVoltage, sizeof(desiredVoltage));
    // String inputString = Serial.readStringUntil('\n'); // Read the input string until a newline character
    // float desiredVoltage = inputString.toFloat();

    // Ensure the desired voltage is within the range
    if (desiredVoltage >= minVoltage && desiredVoltage <= maxVoltage) {
      // Calculate the DAC value for MCP4921
      dacValue = (desiredVoltage / 5.0) * 4095;
      Serial.print("DAC Value: ");
      Serial.println(dacValue);

      // Send the DAC value to the MCP4921
      sendDACValue(dacValue);
    } else {
      Serial.println("Invalid voltage. Enter a value between 0 and 5:");
    }
  }
}

void sendDACValue(int value) {
  digitalWrite(CS_PIN, LOW);

  // Send two bytes (16 bits) to the MCP4921
  SPI.transfer((value >> 8) & 0x0F | 0x30);  // Control bits + upper 4 bits of the value
  SPI.transfer(value & 0xFF);                // Lower 8 bits of the value

  digitalWrite(CS_PIN, HIGH);
}
