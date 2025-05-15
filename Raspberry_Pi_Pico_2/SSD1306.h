#ifndef SSD1306_H
#define SSD1306_H

/**
 * @file SSD1306.h
 * @brief Header file for the SSD1306 OLED display driver
 *
 * This driver provides functionality to control an SSD1306 OLED display
 * over I2C. It supports basic drawing operations and text rendering.
 */

#define SSD1306_WIDTH 128 ///< Display width in pixels
#define SSD1306_HEIGHT 32 ///< Display height in pixels

#define SSD1306_I2C_ADDRESS 0x3C ///< Default I2C address for SSD1306 displays

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "fonts.h"

/**
 * @brief Text alignment options for the display
 */
typedef enum
{
    SSD1306_ALIGN_LEFT = 0, ///< Align text to the left
    SSD1306_ALIGN_CENTER,   ///< Center text horizontally
    SSD1306_ALIGN_RIGHT     ///< Align text to the right
} SSD1306_Align_t;

/**
 * @brief Class to control an SSD1306 OLED display
 *
 * This class provides methods to initialize and control an SSD1306 OLED display
 * over I2C. It supports basic drawing operations like pixels, rectangles, and text
 * rendering with different fonts and alignments.
 */
class SSD1306
{
private:
    const int paddingInline = 4; ///< Horizontal padding for text
    const int paddingBlock = 1;  ///< Vertical padding for text
    i2c_inst_t *i2c;             ///< I2C instance for communication

    // Buffer for display data
    uint8_t *buffer; ///< Display buffer (128x32 pixels = 512 bytes)

    /**
     * @brief Send a command to the display
     * @param command The command byte to send
     */
    void sendCommand(uint8_t command);

    /**
     * @brief Initialize the display with default settings
     */
    void init();

public:
    /**
     * @brief Construct a new SSD1306 object
     *
     * @param i2c_instance Pointer to the I2C instance to use
     * @param sda_pin SDA pin number
     * @param scl_pin SCL pin number
     */
    SSD1306(i2c_inst_t *i2c_instance, uint8_t sda_pin, uint8_t scl_pin);

    /**
     * @brief Destroy the SSD1306 object and free resources
     */
    ~SSD1306();

    /**
     * @brief Clear the display buffer
     *
     * Sets all pixels in the buffer to off (black)
     */
    void clearBuffer();

    /**
     * @brief Draw a single pixel
     *
     * @param x X coordinate (0 to SSD1306_WIDTH-1)
     * @param y Y coordinate (0 to SSD1306_HEIGHT-1)
     * @param white true for white pixel, false for black
     */
    void drawPixel(int16_t x, int16_t y, bool white);

    /**
     * @brief Draw text using a basic 5x7 font
     *
     * @param text The text to draw
     * @param x X coordinate
     * @param y Y coordinate
     * @param white true for white text, false for black
     */
    void drawText(const char *text, int16_t x, int16_t y, bool white);

    /**
     * @brief Draw a rectangle
     *
     * @param x X coordinate of top-left corner
     * @param y Y coordinate of top-left corner
     * @param w Width of rectangle
     * @param h Height of rectangle
     * @param white true for white rectangle, false for black
     * @param fill true to fill the rectangle, false for outline only
     */
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, bool white, bool fill = false);

    /**
     * @brief Draw a single character using the specified font
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param c Character to draw
     * @param font Font to use
     */
    void drawChar(int x, int y, char c, const sFONT &font);

    /**
     * @brief Draw a string with decimal place control
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @param str String to draw
     * @param font Font to use
     * @param decimalPlaces Number of decimal places to display (-1 for all)
     */
    void drawString(int x, int y, const char *str, const sFONT &font, int decimalPlaces = 1);

    /**
     * @brief Draw a string with specified alignment
     *
     * @param align Text alignment (LEFT, CENTER, RIGHT)
     * @param str String to draw
     * @param font Font to use
     */
    void drawString(SSD1306_Align_t align, const char *str, const sFONT &font);

    /**
     * @brief Update the display with the current buffer contents
     *
     * Sends the display buffer to the SSD1306 display over I2C
     */
    void display();
};

#endif // SSD1306_H