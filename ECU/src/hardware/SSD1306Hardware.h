#pragma once

#include "SSD1306.h"
#include <../LCD_display/Fonts/fonts.h>

class SSD1306Hardware : public SSD1306
{
private:
    /* data */

    void init() override;
    void sendCommand(uint8_t) override;
    void display() override;

public:
    SSD1306Hardware();
    ~SSD1306Hardware();
};