/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

Pick one up today in the adafruit shop!
------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

/*********************************************************************
I change the adafruit SSD1306 to SH1106

SH1106 driver doesn't provide several functions such as scroll commands.

*********************************************************************/

#ifndef _SMALL_DISPLAY_H
#define _SMALL_DISPLAY_H

#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_SH1106.h>

#define OLED_RESET 4

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH 16

#define MAX_DATA_TO_DISPLAY 3

static const unsigned char PROGMEM logo16_glcd_bmp[] =
    {B00000000, B11000000,
     B00000001, B11000000,
     B00000001, B11000000,
     B00000011, B11100000,
     B11110011, B11100000,
     B11111110, B11111000,
     B01111110, B11111111,
     B00110011, B10011111,
     B00011111, B11111100,
     B00001101, B01110000,
     B00011011, B10100000,
     B00111111, B11100000,
     B00111111, B11110000,
     B01111100, B11110000,
     B01110000, B01110000,
     B00000000, B00110000};

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif
class SmallDisplay
{
private:
    uint64_t currentMillis;
    uint64_t lastMillis;
    uint64_t refreshInterval;

    const uint8_t defaultTimeDisplayX = 5;
    const uint8_t defaultTimeDisplayY = 0;
    const uint8_t defaultTimeDisplaySize = 4;

    const uint8_t defaultDateDisplayX = 4;
    const uint8_t defaultDateDisplayY = 35;
    const uint8_t defaultDateDisplaySize = 1;

    const uint8_t defaultTempDisplayX = 4;
    const uint8_t defaultTempDisplayY = 50;
    const uint8_t defaultTempDisplaySize = 2;

    const uint8_t defaultHumdtDisplayX = 88;
    const uint8_t defaultHumdtDisplayY = 50;
    const uint8_t defaultHumdtDisplaySize = 2;

public:
    SmallDisplay(/* args */);
    ~SmallDisplay();

    void begin();
    void display(String, String, String, String);
    void drawTorinoLogo();
};

void drawFuelConsumption(float value);
void draw();
void testdrawbitmap(const uint8_t *, uint8_t, uint8_t);
void testdrawchar(void);
void testdrawcircle(void);
void testfillrect(void);
void testdrawtriangle(void);
void testfilltriangle(void);
void testdrawroundrect(void);
void testfillroundrect(void);
void testdrawrect(void);
void testdrawline();

#endif // _SMALL_DISPLAY_H