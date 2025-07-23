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

    // Initialize the display
    init();
}

// Destructor
SSD1306::~SSD1306()
{
    if (buffer != NULL)
    {
        free(buffer);
        buffer = NULL;
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
    // Initialize display with optimized sequence
    const uint8_t init_commands[] = {
        0xAE, // Display off
        0xD5, // Set display clock div
        0x80, // Default value
        0xA8, // Set multiplex
        SSD1306_HEIGHT - 1,
        0xD3, // Set display offset
        0x00, // No offset
        0x40, // Start line
        0x8D, // Charge pump
        0x14, // Enable charge pump
        0x20, // Memory mode
        0x00, // Act like ks0108
        0xA1, // Seg remap
        0xC8, // Com scan dec
        0xDA, // Com pins
        SSD1306_HEIGHT == 32 ? 0x02 : 0x12,
        0x81, // Set contrast
        0xCF,
        0xD9, // Set precharge
        0xF1,
        0xDB, // Set vcom detect
        0x40,
        0xA4, // Display all on resume
        0xA6, // Normal display (not inverted)
        0x2E, // Deactivate scroll
        0xAF  // Display on
    };

    // Send all commands in a single I2C transaction
    for (size_t i = 0; i < sizeof(init_commands); i++)
    {
        sendCommand(init_commands[i]);
    }

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

    // Calculate the byte index and bit position
    uint16_t byte_idx = x + (y / 8) * SSD1306_WIDTH;
    uint8_t bit_pos = y % 8;

    if (white)
    {
        buffer[byte_idx] |= (1 << bit_pos);
    }
    else
    {
        buffer[byte_idx] &= ~(1 << bit_pos);
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
        // Optimize filled rectangle drawing
        for (int16_t j = y; j < y + h; j++)
        {
            for (int16_t i = x; i < x + w; i++)
            {
                drawPixel(i, j, white);
            }
        }
    }
    else
    {
        // Optimize outline rectangle drawing
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
    sendCommand(0x21);
    sendCommand(0);
    sendCommand(SSD1306_WIDTH - 1);

    // Set page address
    sendCommand(0x22);
    sendCommand(0);
    sendCommand(SSD1306_HEIGHT / 8 - 1);

    // Send the buffer in chunks to avoid memory issues
    const uint8_t chunk_size = 32;
    uint8_t *ptr = buffer;
    uint16_t remaining = SSD1306_WIDTH * SSD1306_HEIGHT / 8;

    while (remaining > 0)
    {
        uint8_t current_chunk = (remaining > chunk_size) ? chunk_size : remaining;
        uint8_t *send_buf = (uint8_t *)malloc(current_chunk + 1);

        if (send_buf)
        {
            send_buf[0] = 0x40; // Data command
            memcpy(send_buf + 1, ptr, current_chunk);
            i2c_write_blocking(i2c, SSD1306_I2C_ADDRESS, send_buf, current_chunk + 1, false);
            free(send_buf);

            ptr += current_chunk;
            remaining -= current_chunk;
        }
        else
        {
            break;
        }
    }
}

void SSD1306::drawChar(int x, int y, char c, const sFONT &font)
{
    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
        return;

    uint16_t charOffset = (c - 32) * font.Height * ((font.Width + 7) / 8);
    const uint8_t *charData = &font.table[charOffset];

    // Optimize character drawing by processing bytes instead of individual pixels
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
    int width = strlen(str) * font.Width;
    int x;

    switch (align)
    {
    case SSD1306_ALIGN_CENTER:
        x = (SSD1306_WIDTH - width) / 2 - paddingInline;
        break;
    case SSD1306_ALIGN_RIGHT:
        x = SSD1306_WIDTH - width - paddingInline;
        break;
    default:
        x = 0;
        break;
    }

    drawString(x, -paddingBlock, str, font, 1);
}

void SSD1306::drawString(int x, int y, const char *str, const sFONT &font, int decimalPlaces)
{
    // Clear the buffer
    clearBuffer();

    // Process the string
    const char *ptr = str;
    int decimalCount = 0;
    bool foundDecimal = false;

    while (*ptr)
    {
        // Handle decimal places
        if (foundDecimal)
        {
            decimalCount++;
            if (decimalPlaces >= 0 && decimalCount > decimalPlaces)
            {
                break;
            }
        }
        else if (*ptr == '.')
        {
            foundDecimal = true;
        }

        // Draw character
        drawChar(x, y, *ptr, font);

        // Update position
        x += (*ptr == '.') ? (font.Width - 5) : (font.Width + 1);
        ptr++;
    }

    // Update display
    display();
}
