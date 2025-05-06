#ifndef SSD1306_H
#define SSD1306_H

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32

#define SSD1306_I2C_ADDRESS 0x3C

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "fonts.h"

typedef enum
{
    SSD1306_ALIGN_LEFT = 0,
    SSD1306_ALIGN_CENTER,
    SSD1306_ALIGN_RIGHT
} SSD1306_Align_t;

class SSD1306
{
private:
    const int paddingInline = 4;
    const int paddingBlock = 1;
    i2c_inst_t *i2c;

    // Buffer for display data
    uint8_t *buffer;

    // Send command to the display
    void sendCommand(uint8_t command);

    // Initialize display
    void init();

public:
    // Constructor
    SSD1306(i2c_inst_t *i2c_instance, uint8_t sda_pin, uint8_t scl_pin);

    // Destructor
    ~SSD1306();

    // Clear the buffer
    void clearBuffer();

    // Draw a pixel
    void drawPixel(int16_t x, int16_t y, bool white);

    // Draw text
    void drawText(const char *text, int16_t x, int16_t y, bool white);

    // Draw a rectangle
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, bool white, bool fill = false);

    void drawChar(int, int, char, const sFONT &);
    void drawString(int, int, const char *, const sFONT &);
    void drawString(SSD1306_Align_t, const char *, const sFONT &);

    // Update the display
    void display();
};

#endif // SSD1306_H