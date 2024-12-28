#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h> // For usleep

#include <../LCD_display/Fonts/fonts.h>
#include "../../includes/BCM2835Manager.h"

// Define I2C address of SSD1306 (change if needed)
#define SSD1306_I2C_ADDR 0x3C

// SSD1306 Commands
#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40

// Display dimensions
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32

class SSD1306
{
private:
    /* data */
    // Buffer for each display
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
    void ssd1306_init();
    void ssd1306_sendCommand(uint8_t);
    void ssd1306_clear();
    void ssd1306_drawChar(int, int, char, const sFONT &);
    void ssd1306_drawString(int, int, const char *, const sFONT &);

public:
    SSD1306(/* args */);
    ~SSD1306();
    void ssd1306_display();
};