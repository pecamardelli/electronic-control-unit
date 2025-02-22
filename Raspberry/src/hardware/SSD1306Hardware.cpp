#include "SSD1306Hardware.h"

SSD1306Hardware::SSD1306Hardware()
{
    init();
    clear();
}

SSD1306Hardware::~SSD1306Hardware()
{
}

// Initialize the SSD1306Hardware display
void SSD1306Hardware::init()
{
    i2c_setSlaveAddress(SSD1306_I2C_ADDR);
    sendInitCommands();
}

// Send command to SSD1306Hardware
void SSD1306Hardware::sendCommand(uint8_t cmd)
{
    char data[2] = {SSD1306_COMMAND, cmd};
    i2c_write(data, 2);
}

// Update the display with buffer content
void SSD1306Hardware::display()
{
    i2c_setSlaveAddress(SSD1306_I2C_ADDR);

    sendCommand(0x21); // Set column address
    sendCommand(0x00);
    sendCommand(SSD1306_WIDTH - 1);
    sendCommand(0x22); // Set page address
    sendCommand(0x00);
    sendCommand((SSD1306_HEIGHT / 8) - 1);

    for (uint16_t i = 0; i < sizeof(buffer); i++)
    {
        char data[2] = {SSD1306_DATA, buffer[i]};
        i2c_write(data, 2);
    }
}