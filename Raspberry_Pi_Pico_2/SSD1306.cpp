#include "SSD1306.h"
#include <stdlib.h>
#include <string.h>

// Constructor
SSD1306::SSD1306(i2c_inst_t *i2c_instance, uint8_t sda_pin, uint8_t scl_pin) : i2c(i2c_instance)
{
    // Allocate buffer for the display
    buffer = (uint8_t *)malloc(SSD1306_WIDTH * SSD1306_HEIGHT / 8);
    if (buffer == NULL)
    {
        printf("Failed to allocate display buffer\n");
        return;
    }

    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);

    // Add program information for picotool
    // bi_decl(bi_2pins_with_func(sda_pin, scl_pin, GPIO_FUNC_I2C));

    // Initialize the display
    init();
}

// Destructor
SSD1306::~SSD1306()
{
    if (buffer != NULL)
    {
        free(buffer);
    }
}

// Send command to the display
void SSD1306::sendCommand(uint8_t command)
{
    uint8_t buf[2] = {0x00, command}; // 0x00 indicates command
    i2c_write_blocking(i2c, SSD1306_I2C_ADDRESS, buf, 2, false);
}

// Initialize display
void SSD1306::init()
{
    // Wait a moment for the display to start up
    sleep_ms(100);

    // Initialize display
    sendCommand(0xAE); // Display off
    sendCommand(0xD5); // Set display clock div
    sendCommand(0x80); // Default value
    sendCommand(0xA8); // Set multiplex
    sendCommand(SSD1306_HEIGHT - 1);
    sendCommand(0xD3); // Set display offset
    sendCommand(0x00); // No offset
    sendCommand(0x40); // Start line
    sendCommand(0x8D); // Charge pump
    sendCommand(0x14); // Enable charge pump
    sendCommand(0x20); // Memory mode
    sendCommand(0x00); // Act like ks0108
    sendCommand(0xA1); // Seg remap
    sendCommand(0xC8); // Com scan dec
    sendCommand(0xDA); // Com pins
    sendCommand(SSD1306_HEIGHT == 32 ? 0x02 : 0x12);
    sendCommand(0x81); // Set contrast
    sendCommand(0xCF);
    sendCommand(0xD9); // Set precharge
    sendCommand(0xF1);
    sendCommand(0xDB); // Set vcom detect
    sendCommand(0x40);
    sendCommand(0xA4); // Display all on resume
    sendCommand(0xA6); // Normal display (not inverted)
    sendCommand(0x2E); // Deactivate scroll
    sendCommand(0xAF); // Display on

    // Clear the buffer
    clearBuffer();
}

// Clear the buffer
void SSD1306::clearBuffer()
{
    if (buffer != NULL)
    {
        memset(buffer, 0, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
    }
}

// Draw a pixel
void SSD1306::drawPixel(int16_t x, int16_t y, bool white)
{
    if (x < 0 || x >= SSD1306_WIDTH || y < 0 || y >= SSD1306_HEIGHT || buffer == NULL)
        return;

    // Calculate the byte index
    uint16_t byte_idx = x + (y / 8) * SSD1306_WIDTH;

    // Calculate the bit position within the byte
    uint8_t bit_pos = y % 8;

    if (white)
    {
        buffer[byte_idx] |= (1 << bit_pos); // Set bit
    }
    else
    {
        buffer[byte_idx] &= ~(1 << bit_pos); // Clear bit
    }
}

// Draw text
void SSD1306::drawText(const char *text, int16_t x, int16_t y, bool white)
{
    // Basic font implementation (5x7 font)
    // This would normally include a complete font table
    // For brevity, this example is simplified

    // For each character
    while (*text)
    {
        // Draw a simple rectangle for now as placeholder
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 7; j++)
            {
                drawPixel(x + i, y + j, white);
            }
        }
        x += 6; // Move to next character position
        text++;
    }
}

// Draw a rectangle
void SSD1306::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, bool white, bool fill)
{
    if (fill)
    {
        for (int16_t i = x; i < x + w; i++)
        {
            for (int16_t j = y; j < y + h; j++)
            {
                drawPixel(i, j, white);
            }
        }
    }
    else
    {
        for (int16_t i = x; i < x + w; i++)
        {
            drawPixel(i, y, white);
            drawPixel(i, y + h - 1, white);
        }
        for (int16_t j = y; j < y + h; j++)
        {
            drawPixel(x, j, white);
            drawPixel(x + w - 1, j, white);
        }
    }
}

// Update the display
void SSD1306::display()
{
    if (buffer == NULL)
        return;

    // Set column address
    sendCommand(0x21);              // Set column start and end address
    sendCommand(0);                 // Start at column 0
    sendCommand(SSD1306_WIDTH - 1); // End at last column

    // Set page address
    sendCommand(0x22);                   // Set page start and end address
    sendCommand(0);                      // Start at page 0
    sendCommand(SSD1306_HEIGHT / 8 - 1); // End at last page

    // Send the buffer
    uint8_t *send_buf = (uint8_t *)malloc(1 + SSD1306_WIDTH * SSD1306_HEIGHT / 8); // 1 byte command + data
    if (send_buf)
    {
        send_buf[0] = 0x40; // Data command
        memcpy(send_buf + 1, buffer, SSD1306_WIDTH * SSD1306_HEIGHT / 8);
        i2c_write_blocking(i2c, SSD1306_I2C_ADDRESS, send_buf, SSD1306_WIDTH * SSD1306_HEIGHT / 8 + 1, false);
        free(send_buf);
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

void SSD1306::drawString(int x, int y, const char *str, const sFONT &font)
{
    // Clear the entire buffer
    clearBuffer();

    while (*str)
    {
        drawChar(x, y, *str, font);

        // Advance to the next character (accounting for spacing)
        if (*str == '.')
        {
            x += font.Width - 5; // Adjust for smaller spacing after a period
        }
        else
        {
            x += font.Width + 1; // Standard spacing
        }

        str++;
    }

    // Update the display with the new buffer content
    display();
}
