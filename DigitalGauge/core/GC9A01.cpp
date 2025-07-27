#include "GC9A01.h"
#include <cmath>
#include <cstring>
#include <cstdarg>
#include <cstdio>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GC9A01::GC9A01(spi_inst_t* spi, uint8_t cs, uint8_t dc, uint8_t rst, uint8_t bl) 
    : spi_port(spi), pin_cs(cs), pin_dc(dc), pin_rst(rst), pin_bl(bl) {
}

void GC9A01::writeCommand(uint8_t cmd) {
    gpio_put(pin_dc, 0);  // Command mode
    gpio_put(pin_cs, 0);  // Select device
    spi_write_blocking(spi_port, &cmd, 1);
    gpio_put(pin_cs, 1);  // Deselect device
}

void GC9A01::writeData(uint8_t data) {
    gpio_put(pin_dc, 1);  // Data mode
    gpio_put(pin_cs, 0);  // Select device
    spi_write_blocking(spi_port, &data, 1);
    gpio_put(pin_cs, 1);  // Deselect device
}

void GC9A01::writeData16(uint16_t data) {
    uint8_t buffer[2] = {(uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};
    gpio_put(pin_dc, 1);  // Data mode
    gpio_put(pin_cs, 0);  // Select device
    spi_write_blocking(spi_port, buffer, 2);
    gpio_put(pin_cs, 1);  // Deselect device
}

void GC9A01::writeDataBuffer(uint8_t* buffer, size_t len) {
    gpio_put(pin_dc, 1);  // Data mode
    gpio_put(pin_cs, 0);  // Select device
    spi_write_blocking(spi_port, buffer, len);
    gpio_put(pin_cs, 1);  // Deselect device
}

void GC9A01::init() {
    // Initialize GPIO pins
    gpio_init(pin_cs);
    gpio_init(pin_dc);
    gpio_init(pin_rst);
    
    gpio_set_dir(pin_cs, GPIO_OUT);
    gpio_set_dir(pin_dc, GPIO_OUT);
    gpio_set_dir(pin_rst, GPIO_OUT);
    
    gpio_put(pin_cs, 1);   // Deselect initially
    gpio_put(pin_dc, 0);   // Command mode initially
    
    // Initialize backlight if configured
    if (pin_bl != 255) {
        gpio_init(pin_bl);
        gpio_set_dir(pin_bl, GPIO_OUT);
        gpio_put(pin_bl, 1);  // Turn on backlight
    }
    
    reset();
    
    // GC9A01 specific initialization sequence
    writeCommand(0xEF);
    writeCommand(0xEB);
    writeData(0x14);
    
    writeCommand(0xFE);
    writeCommand(0xEF);
    
    writeCommand(0xEB);
    writeData(0x14);
    
    writeCommand(0x84);
    writeData(0x40);
    
    writeCommand(0x85);
    writeData(0xFF);
    
    writeCommand(0x86);
    writeData(0xFF);
    
    writeCommand(0x87);
    writeData(0xFF);
    
    writeCommand(0x88);
    writeData(0x0A);
    
    writeCommand(0x89);
    writeData(0x21);
    
    writeCommand(0x8A);
    writeData(0x00);
    
    writeCommand(0x8B);
    writeData(0x80);
    
    writeCommand(0x8C);
    writeData(0x01);
    
    writeCommand(0x8D);
    writeData(0x01);
    
    writeCommand(0x8E);
    writeData(0xFF);
    
    writeCommand(0x8F);
    writeData(0xFF);
    
    writeCommand(0xB6);
    writeData(0x00);
    writeData(0x20);
    
    writeCommand(0x36);
    writeData(0x08);
    
    writeCommand(0x3A);
    writeData(0x05);
    
    writeCommand(0x90);
    writeData(0x08);
    writeData(0x08);
    writeData(0x08);
    writeData(0x08);
    
    writeCommand(0xBD);
    writeData(0x06);
    
    writeCommand(0xBC);
    writeData(0x00);
    
    writeCommand(0xFF);
    writeData(0x60);
    writeData(0x01);
    writeData(0x04);
    
    writeCommand(0xC3);
    writeData(0x13);
    writeCommand(0xC4);
    writeData(0x13);
    
    writeCommand(0xC9);
    writeData(0x22);
    
    writeCommand(0xBE);
    writeData(0x11);
    
    writeCommand(0xE1);
    writeData(0x10);
    writeData(0x0E);
    
    writeCommand(0xDF);
    writeData(0x21);
    writeData(0x0c);
    writeData(0x02);
    
    writeCommand(0xF0);
    writeData(0x45);
    writeData(0x09);
    writeData(0x08);
    writeData(0x08);
    writeData(0x26);
    writeData(0x2A);
    
    writeCommand(0xF1);
    writeData(0x43);
    writeData(0x70);
    writeData(0x72);
    writeData(0x36);
    writeData(0x37);
    writeData(0x6F);
    
    writeCommand(0xF2);
    writeData(0x45);
    writeData(0x09);
    writeData(0x08);
    writeData(0x08);
    writeData(0x26);
    writeData(0x2A);
    
    writeCommand(0xF3);
    writeData(0x43);
    writeData(0x70);
    writeData(0x72);
    writeData(0x36);
    writeData(0x37);
    writeData(0x6F);
    
    writeCommand(0xED);
    writeData(0x1B);
    writeData(0x0B);
    
    writeCommand(0xAE);
    writeData(0x77);
    
    writeCommand(0xCD);
    writeData(0x63);
    
    writeCommand(0x70);
    writeData(0x07);
    writeData(0x07);
    writeData(0x04);
    writeData(0x0E);
    writeData(0x0F);
    writeData(0x09);
    writeData(0x07);
    writeData(0x08);
    writeData(0x03);
    
    writeCommand(0xE8);
    writeData(0x34);
    
    writeCommand(0x62);
    writeData(0x18);
    writeData(0x0D);
    writeData(0x71);
    writeData(0xED);
    writeData(0x70);
    writeData(0x70);
    writeData(0x18);
    writeData(0x0F);
    writeData(0x71);
    writeData(0xEF);
    writeData(0x70);
    writeData(0x70);
    
    writeCommand(0x63);
    writeData(0x18);
    writeData(0x11);
    writeData(0x71);
    writeData(0xF1);
    writeData(0x70);
    writeData(0x70);
    writeData(0x18);
    writeData(0x13);
    writeData(0x71);
    writeData(0xF3);
    writeData(0x70);
    writeData(0x70);
    
    writeCommand(0x64);
    writeData(0x28);
    writeData(0x29);
    writeData(0xF1);
    writeData(0x01);
    writeData(0xF1);
    writeData(0x00);
    writeData(0x07);
    
    writeCommand(0x66);
    writeData(0x3C);
    writeData(0x00);
    writeData(0xCD);
    writeData(0x67);
    writeData(0x45);
    writeData(0x45);
    writeData(0x10);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);
    
    writeCommand(0x67);
    writeData(0x00);
    writeData(0x3C);
    writeData(0x00);
    writeData(0x00);
    writeData(0x00);
    writeData(0x01);
    writeData(0x54);
    writeData(0x10);
    writeData(0x32);
    writeData(0x98);
    
    writeCommand(0x74);
    writeData(0x10);
    writeData(0x85);
    writeData(0x80);
    writeData(0x00);
    writeData(0x00);
    writeData(0x4E);
    writeData(0x00);
    
    writeCommand(0x98);
    writeData(0x3e);
    writeData(0x07);
    
    writeCommand(0x35);
    writeCommand(0x21);
    
    writeCommand(0x11);
    sleep_ms(120);
    writeCommand(0x29);
    sleep_ms(20);
    
    fillScreen(BLACK);
}

void GC9A01::reset() {
    gpio_put(pin_rst, 0);
    sleep_ms(10);
    gpio_put(pin_rst, 1);
    sleep_ms(120);
}

void GC9A01::setBacklight(bool on) {
    if (pin_bl != 255) {
        gpio_put(pin_bl, on ? 1 : 0);
    }
}

void GC9A01::setRotation(uint8_t rotation) {
    writeCommand(GC9A01_MADCTL);
    switch (rotation % 4) {
        case 0:
            writeData(0x08);
            break;
        case 1:
            writeData(0x68);
            break;
        case 2:
            writeData(0xC8);
            break;
        case 3:
            writeData(0xA8);
            break;
    }
}

void GC9A01::invertDisplay(bool invert) {
    writeCommand(invert ? GC9A01_INVON : GC9A01_INVOFF);
}

void GC9A01::displayOn() {
    writeCommand(GC9A01_DISPON);
}

void GC9A01::displayOff() {
    writeCommand(GC9A01_DISPOFF);
}

void GC9A01::setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(GC9A01_CASET);
    writeData16(x0);
    writeData16(x1);
    
    writeCommand(GC9A01_RASET);
    writeData16(y0);
    writeData16(y1);
    
    writeCommand(GC9A01_RAMWR);
}

void GC9A01::fillScreen(uint16_t color) {
    fillRect(0, 0, GC9A01_WIDTH, GC9A01_HEIGHT, color);
}

void GC9A01::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x >= GC9A01_WIDTH || y >= GC9A01_HEIGHT) return;
    
    setAddressWindow(x, y, x, y);
    writeData16(color);
}

void GC9A01::swap(uint16_t& a, uint16_t& b) {
    uint16_t temp = a;
    a = b;
    b = temp;
}

void GC9A01::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }
    
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    
    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep = (y0 < y1) ? 1 : -1;
    
    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void GC9A01::drawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color) {
    fillRect(x, y, w, 1, color);
}

void GC9A01::drawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color) {
    fillRect(x, y, 1, h, color);
}

void GC9A01::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    drawHLine(x, y, w, color);
    drawHLine(x, y + h - 1, w, color);
    drawVLine(x, y, h, color);
    drawVLine(x + w - 1, y, h, color);
}

void GC9A01::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if (x >= GC9A01_WIDTH || y >= GC9A01_HEIGHT) return;
    
    if (x + w > GC9A01_WIDTH) w = GC9A01_WIDTH - x;
    if (y + h > GC9A01_HEIGHT) h = GC9A01_HEIGHT - y;
    
    setAddressWindow(x, y, x + w - 1, y + h - 1);
    
    uint8_t buffer[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    gpio_put(pin_dc, 1);  // Data mode
    gpio_put(pin_cs, 0);  // Select device
    
    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        spi_write_blocking(spi_port, buffer, 2);
    }
    
    gpio_put(pin_cs, 1);  // Deselect device
}

void GC9A01::drawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

void GC9A01::fillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    drawVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}

void GC9A01::drawCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            drawPixel(x0 - y, y0 - x, color);
            drawPixel(x0 - x, y0 - y, color);
        }
    }
}

void GC9A01::fillCircleHelper(uint16_t x0, uint16_t y0, uint16_t r, uint8_t cornername, uint16_t delta, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    
    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        
        if (cornername & 0x1) {
            drawVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
            drawVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
        }
        if (cornername & 0x2) {
            drawVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
            drawVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
        }
    }
}

void GC9A01::drawRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color) {
    drawHLine(x + r, y, w - 2 * r, color);
    drawHLine(x + r, y + h - 1, w - 2 * r, color);
    drawVLine(x, y + r, h - 2 * r, color);
    drawVLine(x + w - 1, y + r, h - 2 * r, color);
    
    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
}

void GC9A01::fillRoundRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t r, uint16_t color) {
    fillRect(x + r, y, w - 2 * r, h, color);
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
}

void GC9A01::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

void GC9A01::fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int16_t a, b, y, last;
    
    if (y0 > y1) {
        swap(y0, y1); swap(x0, x1);
    }
    if (y1 > y2) {
        swap(y2, y1); swap(x2, x1);
    }
    if (y0 > y1) {
        swap(y0, y1); swap(x0, x1);
    }
    
    if (y0 == y2) {
        a = b = x0;
        if (x1 < a) a = x1;
        else if (x1 > b) b = x1;
        if (x2 < a) a = x2;
        else if (x2 > b) b = x2;
        drawHLine(a, y0, b - a + 1, color);
        return;
    }
    
    int16_t dx01 = x1 - x0;
    int16_t dy01 = y1 - y0;
    int16_t dx02 = x2 - x0;
    int16_t dy02 = y2 - y0;
    int16_t dx12 = x2 - x1;
    int16_t dy12 = y2 - y1;
    
    int32_t sa = 0;
    int32_t sb = 0;
    
    if (y1 == y2) last = y1;
    else last = y1 - 1;
    
    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        
        if (a > b) {
            int16_t temp = a; a = b; b = temp;
        }
        drawHLine(a, y, b - a + 1, color);
    }
    
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        
        if (a > b) {
            int16_t temp = a; a = b; b = temp;
        }
        drawHLine(a, y, b - a + 1, color);
    }
}

void GC9A01::drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, sFONT* font) {
    if (c < 32 || c > 126) c = 32; // Replace non-printable with space
    
    uint32_t char_offset = (c - 32) * font->Height * ((font->Width + 7) / 8);
    const uint8_t* char_data = &font->table[char_offset];
    
    for (uint16_t row = 0; row < font->Height; row++) {
        for (uint16_t col = 0; col < font->Width; col++) {
            // Calculate byte and bit position
            uint16_t byte_index = row * ((font->Width + 7) / 8) + (col / 8);
            uint8_t bit_position = 7 - (col % 8);
            
            // Check if pixel should be drawn
            if (char_data[byte_index] & (1 << bit_position)) {
                drawPixel(x + col, y + row, color);
            } else if (bg != color) {
                drawPixel(x + col, y + row, bg);
            }
        }
    }
}

uint16_t GC9A01::getCharSpacing(char c, sFONT* font) {
    // Character-specific spacing adjustments - only reduce spacing for narrow characters
    switch (c) {
        case ' ':  // Space
            return font->Width * 0.6f;  // Slightly smaller space
        case '.':  // Period
        case ',':  // Comma
        case ':':  // Colon
        case ';':  // Semicolon
        case '\'': // Apostrophe
        case '!':  // Exclamation
            return font->Width * 0.5f;  // Much narrower - these are very thin
        case 'I':  // Capital I
        case 'i':  // Lowercase i
        case 'l':  // Lowercase l
        case '|':  // Pipe
            return font->Width * 0.6f;  // Narrower - these are thin
        case 'f':  // Lowercase f
        case 'j':  // Lowercase j
        case 't':  // Lowercase t
            return font->Width * 0.8f;  // Slightly narrower
        default:
            return font->Width;  // Standard width for all other characters
    }
}

void GC9A01::print(uint16_t x, uint16_t y, const char* text, uint16_t color, uint16_t bg, sFONT* font) {
    uint16_t currentX = x;
    while (*text) {
        if (*text == '\n') {
            currentX = x;
            y += font->Height;
        } else {
            drawChar(currentX, y, *text, color, bg, font);
            currentX += getCharSpacing(*text, font);
        }
        text++;
    }
}

void GC9A01::printf(uint16_t x, uint16_t y, uint16_t color, uint16_t bg, sFONT* font, const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    print(x, y, buffer, color, bg, font);
}

void GC9A01::drawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const unsigned char* image_data) {
    if (x >= GC9A01_WIDTH || y >= GC9A01_HEIGHT) return;
    
    // Clip to screen boundaries
    if (x + w > GC9A01_WIDTH) w = GC9A01_WIDTH - x;
    if (y + h > GC9A01_HEIGHT) h = GC9A01_HEIGHT - y;
    
    setAddressWindow(x, y, x + w - 1, y + h - 1);
    
    // Send image data directly (assuming RGB565 format)
    gpio_put(pin_dc, 1);  // Data mode
    gpio_put(pin_cs, 0);  // Select device
    
    // Send image data in chunks to avoid potential SPI buffer issues
    const size_t chunk_size = 1024;  // Send 1KB at a time
    size_t total_bytes = w * h * 2;  // 2 bytes per pixel for RGB565
    
    for (size_t offset = 0; offset < total_bytes; offset += chunk_size) {
        size_t bytes_to_send = (offset + chunk_size > total_bytes) ? (total_bytes - offset) : chunk_size;
        spi_write_blocking(spi_port, &image_data[offset], bytes_to_send);
    }
    
    gpio_put(pin_cs, 1);  // Deselect device
}

uint16_t GC9A01::color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

uint16_t GC9A01::interpolateColor(uint16_t color1, uint16_t color2, float factor) {
    if (factor <= 0.0f) return color1;
    if (factor >= 1.0f) return color2;
    
    uint8_t r1 = (color1 >> 11) & 0x1F;
    uint8_t g1 = (color1 >> 5) & 0x3F;
    uint8_t b1 = color1 & 0x1F;
    
    uint8_t r2 = (color2 >> 11) & 0x1F;
    uint8_t g2 = (color2 >> 5) & 0x3F;
    uint8_t b2 = color2 & 0x1F;
    
    uint8_t r = r1 + (uint8_t)((r2 - r1) * factor);
    uint8_t g = g1 + (uint8_t)((g2 - g1) * factor);
    uint8_t b = b1 + (uint8_t)((b2 - b1) * factor);
    
    return (r << 11) | (g << 5) | b;
}

void GC9A01::drawGauge(uint16_t centerX, uint16_t centerY, uint16_t radius, 
                       float value, float minVal, float maxVal, 
                       uint16_t needleColor, uint16_t scaleColor) {
    // Draw outer circle
    drawCircle(centerX, centerY, radius, scaleColor);
    drawCircle(centerX, centerY, radius - 1, scaleColor);
    
    // Calculate needle angle (270 degrees range, starting from -135 degrees)
    float angleRange = 270.0f;  // degrees
    float startAngle = -135.0f; // degrees
    float normalizedValue = (value - minVal) / (maxVal - minVal);
    if (normalizedValue < 0) normalizedValue = 0;
    if (normalizedValue > 1) normalizedValue = 1;
    
    float needleAngle = startAngle + (normalizedValue * angleRange);
    float needleRad = needleAngle * M_PI / 180.0f;
    
    // Draw needle
    uint16_t needleLength = radius - 10;
    int16_t needleX = centerX + (int16_t)(needleLength * cos(needleRad));
    int16_t needleY = centerY + (int16_t)(needleLength * sin(needleRad));
    
    // Draw thick needle
    drawLine(centerX, centerY, needleX, needleY, needleColor);
    drawLine(centerX - 1, centerY, needleX - 1, needleY, needleColor);
    drawLine(centerX + 1, centerY, needleX + 1, needleY, needleColor);
    drawLine(centerX, centerY - 1, needleX, needleY - 1, needleColor);
    drawLine(centerX, centerY + 1, needleX, needleY + 1, needleColor);
    
    // Draw center dot
    fillCircle(centerX, centerY, 4, needleColor);
    
    // Draw scale marks
    for (int i = 0; i <= 10; i++) {
        float markAngle = startAngle + (i * angleRange / 10.0f);
        float markRad = markAngle * M_PI / 180.0f;
        
        uint16_t outerX = centerX + (uint16_t)((radius - 5) * cos(markRad));
        uint16_t outerY = centerY + (uint16_t)((radius - 5) * sin(markRad));
        uint16_t innerX = centerX + (uint16_t)((radius - 15) * cos(markRad));
        uint16_t innerY = centerY + (uint16_t)((radius - 15) * sin(markRad));
        
        drawLine(innerX, innerY, outerX, outerY, scaleColor);
    }
}

void GC9A01::drawArc(uint16_t x, uint16_t y, uint16_t r, float startAngle, float endAngle, uint16_t color, uint8_t thickness) {
    float step = 1.0f / r; // Adaptive step size
    
    for (float angle = startAngle; angle <= endAngle; angle += step) {
        float radian = angle * M_PI / 180.0f;
        for (uint8_t t = 0; t < thickness; t++) {
            uint16_t px = x + (uint16_t)((r - t) * cos(radian));
            uint16_t py = y + (uint16_t)((r - t) * sin(radian));
            drawPixel(px, py, color);
        }
    }
}
