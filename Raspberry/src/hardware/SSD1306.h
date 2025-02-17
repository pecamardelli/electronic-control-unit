#pragma once

#include <iostream>
#include <cstring>

#include "BCM_2835.h"
#include <../LCD_display/Fonts/fonts.h>
#include "TCA9548A.h"

// Define I2C address of SSD1306 (change if needed)
#define SSD1306_I2C_ADDR 0x3C

// SSD1306 Commands
#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40

// Display dimensions
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32

typedef enum
{
    SSD1306_ALIGN_LEFT = 0,
    SSD1306_ALIGN_CENTER,
    SSD1306_ALIGN_RIGHT
} SSD1306_Align_t;

class SSD1306
{
private:
    /* data */
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
    TCA9548A &i2cMultiplexer;
    uint8_t i2cMultiplexerChannel;

    const int paddingInline = 4;
    const int paddingBlock = 1;
    std::string currentString = "";
    char currentScreen[SSD1306_HEIGHT / 31][SSD1306_WIDTH / (15 + 1)]; // Adjust fontWidth and fontHeight as needed
    void init();
    void sendCommand(uint8_t);
    void clear();
    void display();
    void drawChar(int, int, char, const sFONT &);
    void setPixel(int x, int y, bool on);

public:
    SSD1306(TCA9548A &, uint8_t);
    ~SSD1306();

    void drawString(int, int, const char *, const sFONT &);
    void drawString(SSD1306_Align_t, const char *, const sFONT &);
    void drawRectangle(int x, int y, int width, int height, int thickness);
    void drawHorizontalLine(int x, int y, int length);
    void drawVerticalLine(int x, int y, int length);
};