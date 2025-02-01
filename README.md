# ESP32 Dual DAC control boards

These boards are meant to replicate analog signals sent by anything not controlled over the can bus or with pwm. There is one main file esp32_dual_dac_base.ino, that you may build any special behaviors off of.

## Board Layout
The current layout consists of 1 esp32 dev kit and 2 MCP4921 DACs.
![IMG_0028](https://github.com/user-attachments/assets/cabfe5a5-211a-4591-a979-a8bef3a565cf)
This is the (terrible and temporary) diagram of the board layout. Both DACs share the same clock, data pin and share a common ground with the esp32, but have different chip select pins and are powered seperately. 
