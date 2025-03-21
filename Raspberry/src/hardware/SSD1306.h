#pragma once

#include <iostream>
#include <cstring>

#include <bcm2835.h>
#include <../LCD_display/Fonts/fonts.h>

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
    const int paddingInline = 4;
    const int paddingBlock = 1;

public:
    SSD1306();
    ~SSD1306();

    void drawString(int, int, const char *, const sFONT &);
    void drawString(SSD1306_Align_t, const char *, const sFONT &);
    void drawRectangle(int x, int y, int width, int height, int thickness);
    void drawHorizontalLine(int x, int y, int length);
    void drawVerticalLine(int x, int y, int length);

protected:
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
    virtual void init() = 0;
    virtual void sendCommand(uint8_t) = 0;
    virtual void display() = 0;

    void clear();
    void sendInitCommands();
    void drawChar(int, int, char, const sFONT &);
    void setPixel(int x, int y, bool on);
};