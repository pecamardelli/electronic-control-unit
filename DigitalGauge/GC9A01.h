#ifndef GC9A01_H
#define GC9A01_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

// Display dimensions
#define GC9A01_WIDTH  240
#define GC9A01_HEIGHT 240

// Color definitions (RGB565)
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410
#define ORANGE  0xFD20
#define PINK    0xF81F
#define PURPLE  0x8010

// GC9A01 Commands
#define GC9A01_SWRESET     0x01
#define GC9A01_SLPOUT      0x11
#define GC9A01_INVOFF      0x20
#define GC9A01_INVON       0x21
#define GC9A01_DISPOFF     0x28
#define GC9A01_DISPON      0x29
#define GC9A01_CASET       0x2A
#define GC9A01_RASET       0x2B
#define GC9A01_RAMWR       0x2C
#define GC9A01_COLMOD      0x3A
#define GC9A01_MADCTL      0x36
#define GC9A01_DFUNCTR     0xB6

class GC9A01 {
private:
    spi_inst_t* spi_port;
    uint8_t pin_cs;
    uint8_t pin_dc;
    uint8_t pin_rst;
    uint8_t pin_bl;
    
    // Low-level SPI communication
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeData16(uint16_t data);
    void writeDataBuffer(uint8_t* buffer, size_t len);
    
public:
    // Constructor
    GC9A01(spi_inst_t* spi, uint8_t cs, uint8_t dc, uint8_t rst, uint8_t bl = 255);
    
    // Initialization and control
    void init();
    void reset();
    void setBacklight(bool on);
    void setRotation(uint8_t rotation);
    void invertDisplay(bool invert);
    void displayOn();
    void displayOff();
    
    // Address window and basic drawing
    void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void fillScreen(uint16_t color);
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    
    // Basic shapes
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
    void drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
    void drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
    void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    
    // Circles
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
    void drawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t color);
    void fillCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t delta, uint16_t color);
    
    // Rounded rectangles
    void drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
    void fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color);
    
    // Triangles
    void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
    void fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
    
    // Bitmap drawing
    void drawBitmap(uint16_t x, uint16_t y, const uint8_t* bitmap, uint16_t w, uint16_t h, uint16_t color);
    void drawBitmap565(uint16_t x, uint16_t y, const uint16_t* bitmap, uint16_t w, uint16_t h);
    
    // Text drawing (basic)
    void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size);
    void print(uint16_t x, uint16_t y, const char* text, uint16_t color, uint16_t bg = BLACK, uint8_t size = 1);
    void printf(uint16_t x, uint16_t y, uint16_t color, uint16_t bg, uint8_t size, const char* format, ...);
    
    // Gauge-specific functions
    void drawGauge(uint16_t centerX, uint16_t centerY, uint16_t radius, 
                   float value, float minVal, float maxVal, 
                   uint16_t needleColor = RED, uint16_t scaleColor = WHITE);
    void drawArc(uint16_t x, uint16_t y, uint16_t r, float startAngle, float endAngle, uint16_t color, uint8_t thickness = 1);
    void drawGradientArc(uint16_t x, uint16_t y, uint16_t r, float startAngle, float endAngle, 
                        uint16_t startColor, uint16_t endColor, uint8_t thickness = 1);
    
    // Color utilities
    uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
    uint16_t interpolateColor(uint16_t color1, uint16_t color2, float factor);
    
    // Utility functions
    uint16_t getWidth() { return GC9A01_WIDTH; }
    uint16_t getHeight() { return GC9A01_HEIGHT; }
    void swap(uint16_t& a, uint16_t& b);
};

#endif // GC9A01_H
