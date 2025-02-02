# ESP32 Dual DAC control boards

These boards are meant to replicate analog signals sent by anything not controlled over the can bus or with pwm. There is one main file [esp32_dual_dac_base.ino](https://github.com/Cavalla-io/esp32_dualDAC_control_board/blob/main/esp32_double_dac_base.ino), that you may build any special behaviors off of.

## Board Layout
The current layout consists of 1 esp32 dev kit and 2 MCP4921 DACs.
![IMG_0029](https://github.com/user-attachments/assets/13630f50-2144-4f8a-ad32-712689644023)
This is the (terrible and temporary) diagram of the board layout. Both DACs share the same clock, data pin and share a common ground with the esp32, but have different chip select pins and are powered seperately. 

## [esp32_dual_dac.ino](https://github.com/Cavalla-io/esp32_dualDAC_control_board/blob/main/esp32_double_dac_base.ino)
***This file is the one you will build any custom behavior off of.***

### Setup
```cpp
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
```
In this section we first import our only dependancy ([the MCP_DAC library](https://github.com/RobTillaart/MCP_DAC))
```cpp
#include "SPI.h"
```
Then we define our chip select pins and start the serial communication.
```cpp
#define MCP4921_CS1_PIN  5  // Chip Select for DAC 1
#define MCP4921_CS2_PIN  4  // Chip Select for DAC 2
```
We then set the start communication with the DACs by sending them an initialization signal. This signal also selects the pins for each wire on the esp32 
```cpp
pinMode(MCP4921_CS1_PIN, OUTPUT);
pinMode(MCP4921_CS2_PIN, OUTPUT);
digitalWrite(MCP4921_CS1_PIN, HIGH);
digitalWrite(MCP4921_CS2_PIN, HIGH);

SPI.begin(18, -1, 23, -1) // (Clock, MISO (disabled), MOSI, SS (disabled))
```
### Message Structure
```cpp
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
```
This section constructs the message that is sent to the DACs

We start by passing in two values, csPin, and value (chip select, voltage to be sent)
```cpp
void mcp4921(uint8_t csPin, uint16_t value)
{
...
}
```
The csPin (Chip Select) is set to LOW to enable communication with the selected DAC.\
The data variable constructs the 16-bit SPI command:
- 0x3000 sets the control bits:
- Bit 15 (1) → Start bit (Always 1)
- Bit 14 (1) → Set to enable DAC output (1 for active mode)
- Bit 13 (0) → Sets the DAC to unbuffered mode (faster response)
- Bit 12 (0) → Sets gain (0 = 1×, 1 = 2×)
- | value appends the 12-bit DAC value (0-4095).
```cpp
uint16_t data = 0x3000 | value;
digitalWrite(csPin, LOW);
```
todo
```cpp
SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));  // 1 MHz SPI
SPI.transfer((uint8_t)(data >> 8));
SPI.transfer((uint8_t)(data & 0xFF));
SPI.endTransaction();
```
todo
```cpp
digitalWrite(csPin, HIGH);
```

