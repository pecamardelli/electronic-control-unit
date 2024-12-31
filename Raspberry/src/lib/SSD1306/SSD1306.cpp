#include "SSD1306.h"

SSD1306::SSD1306(/* args */)
{
    BCM2835Manager::getInstance();
    ssd1306_init();
    ssd1306_clear();

    ssd1306_drawString(30, 0, "1366.8", LiberationSansNarrow_Bold28);
    ssd1306_display();

    bcm2835_delay(5000); // Display message for 5 seconds
}

SSD1306::~SSD1306()
{
}

// Initialize the SSD1306 display
void SSD1306::ssd1306_init()
{
    bcm2835_i2c_setSlaveAddress(SSD1306_I2C_ADDR);

    ssd1306_sendCommand(0xAE); // Display OFF
    ssd1306_sendCommand(0xD5); // Set Display Clock Divide Ratio
    ssd1306_sendCommand(0x80); // Suggested ratio
    ssd1306_sendCommand(0xA8); // Set Multiplex Ratio
    ssd1306_sendCommand(0x1F); // Height - 1
    ssd1306_sendCommand(0xD3); // Set Display Offset
    ssd1306_sendCommand(0x00); // No offset
    ssd1306_sendCommand(0x40); // Set Start Line at 0
    ssd1306_sendCommand(0x8D); // Enable charge pump regulator
    ssd1306_sendCommand(0x14);
    ssd1306_sendCommand(0x20); // Set Memory Addressing Mode
    ssd1306_sendCommand(0x00); // Horizontal addressing
    ssd1306_sendCommand(0xA1); // Set Segment Re-map
    ssd1306_sendCommand(0xC8); // Set COM Output Scan Direction
    ssd1306_sendCommand(0xDA); // Set COM Pins Hardware Configuration
    ssd1306_sendCommand(0x02);
    ssd1306_sendCommand(0x81); // Set Contrast
    ssd1306_sendCommand(0x8F);
    ssd1306_sendCommand(0xD9); // Set Pre-charge Period
    ssd1306_sendCommand(0xF1);
    ssd1306_sendCommand(0xDB); // Set VCOMH Deselect Level
    ssd1306_sendCommand(0x40);
    ssd1306_sendCommand(0xA4); // Disable Entire Display On
    ssd1306_sendCommand(0xA6); // Normal Display
    ssd1306_sendCommand(0x2E); // Deactivate Scroll
    ssd1306_sendCommand(0xAF); // Display ON
}

// Send command to SSD1306
void SSD1306::ssd1306_sendCommand(uint8_t cmd)
{
    char data[2] = {SSD1306_COMMAND, cmd};
    bcm2835_i2c_write(data, 2);
}

// Clear the display buffer
void SSD1306::ssd1306_clear()
{
    std::memset(buffer, 0, sizeof(buffer));
}

// Update the display with buffer content
void SSD1306::ssd1306_display()
{
    ssd1306_sendCommand(0x21); // Set column address
    ssd1306_sendCommand(0x00);
    ssd1306_sendCommand(SSD1306_WIDTH - 1);
    ssd1306_sendCommand(0x22); // Set page address
    ssd1306_sendCommand(0x00);
    ssd1306_sendCommand((SSD1306_HEIGHT / 8) - 1);

    for (uint16_t i = 0; i < sizeof(buffer); i++)
    {
        char data[2] = {SSD1306_DATA, buffer[i]};
        bcm2835_i2c_write(data, 2);
    }
}

// Draw a character (very basic, fixed-width 5x8 font)
void SSD1306::ssd1306_drawChar(int x, int y, char c, const sFONT &font)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
        return;

    uint16_t charOffset = (c - 32) * font.Height * ((font.Width + 7) / 8);
    const uint8_t *charData = &font.table[charOffset];

    for (uint16_t i = 0; i < font.Height; i++)
    {
        for (uint16_t j = 0; j < font.Width; j++)
        {
            uint8_t pixel = (charData[i * ((font.Width + 7) / 8) + (j / 8)] >> (7 - (j % 8))) & 0x01;
            if (pixel)
            {
                int byteIndex = x + j + ((y + i) / 8) * SSD1306_WIDTH;
                int bitIndex = (y + i) % 8;
                buffer[byteIndex] |= (1 << bitIndex);
            }
        }
    }
}

// Draw a string
void SSD1306::ssd1306_drawString(int x, int y, const char *str, const sFONT &font)
{
    while (*str)
    {
        ssd1306_drawChar(x, y, *str, font);
        // Advance to next character (font width + spacing)
        if (strcmp(str, ".") == 0)
        {
            std::cout << "Dot" << std::endl;
            x += font.Width - 7;
        }
        else
            x += font.Width + 1;

        if (x + font.Width > SSD1306_WIDTH)
        {
            x = 0;
            y += font.Height;
        }
        str++;
    }
}