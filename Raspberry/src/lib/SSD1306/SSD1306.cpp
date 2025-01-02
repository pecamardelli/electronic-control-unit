#include "SSD1306.h"

SSD1306::SSD1306(/* args */)
{
    BCM2835Manager::getInstance();
    init();
    clear();
}

SSD1306::~SSD1306()
{
}

// Initialize the SSD1306 display
void SSD1306::init()
{
    bcm2835_i2c_setSlaveAddress(SSD1306_I2C_ADDR);

    sendCommand(0xAE); // Display OFF
    sendCommand(0xD5); // Set Display Clock Divide Ratio
    sendCommand(0x80); // Suggested ratio
    sendCommand(0xA8); // Set Multiplex Ratio
    sendCommand(0x1F); // Height - 1
    sendCommand(0xD3); // Set Display Offset
    sendCommand(0x00); // No offset
    sendCommand(0x40); // Set Start Line at 0
    sendCommand(0x8D); // Enable charge pump regulator
    sendCommand(0x14);
    sendCommand(0x20); // Set Memory Addressing Mode
    sendCommand(0x00); // Horizontal addressing
    sendCommand(0xA1); // Set Segment Re-map
    sendCommand(0xC8); // Set COM Output Scan Direction
    sendCommand(0xDA); // Set COM Pins Hardware Configuration
    sendCommand(0x02);
    sendCommand(0x81); // Set Contrast
    sendCommand(0x8F);
    sendCommand(0xD9); // Set Pre-charge Period
    sendCommand(0xF1);
    sendCommand(0xDB); // Set VCOMH Deselect Level
    sendCommand(0x40);
    sendCommand(0xA4); // Disable Entire Display On
    sendCommand(0xA6); // Normal Display
    sendCommand(0x2E); // Deactivate Scroll
    sendCommand(0xAF); // Display ON
}

// Send command to SSD1306
void SSD1306::sendCommand(uint8_t cmd)
{
    char data[2] = {SSD1306_COMMAND, cmd};
    bcm2835_i2c_write(data, 2);
}

// Clear the display buffer
void SSD1306::clear()
{
    std::memset(buffer, 0, sizeof(buffer));
}

// Update the display with buffer content
void SSD1306::display()
{
    sendCommand(0x21); // Set column address
    sendCommand(0x00);
    sendCommand(SSD1306_WIDTH - 1);
    sendCommand(0x22); // Set page address
    sendCommand(0x00);
    sendCommand((SSD1306_HEIGHT / 8) - 1);

    for (uint16_t i = 0; i < sizeof(buffer); i++)
    {
        char data[2] = {SSD1306_DATA, buffer[i]};
        bcm2835_i2c_write(data, 2);
    }
}

void SSD1306::drawChar(int x, int y, char c, const sFONT &font)
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
void SSD1306::drawString(SSD1306_Align_t align, const char *str, const sFONT &font)
{
    switch (align)
    {
    case SSD1306_ALIGN_CENTER:
        drawString((SSD1306_WIDTH - (strlen(str) * font.Width)) / 2 - paddingInline, -paddingBlock, str, font);
        break;
    case SSD1306_ALIGN_RIGHT:
        drawString(SSD1306_WIDTH - (strlen(str) * font.Width) - paddingInline, -paddingBlock, str, font);
        break;
    default:
        drawString(0, 0, str, font);
        break;
    }
}

// Draw a string
void SSD1306::drawString(int x, int y, const char *str, const sFONT &font)
{
    clear();

    while (*str)
    {
        drawChar(x, y, *str, font);
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

    display();
}