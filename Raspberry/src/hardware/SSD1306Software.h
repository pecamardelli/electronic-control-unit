#pragma once

#include <math.h>
#include <sstream>
#include <iomanip>

#include "SSD1306.h"
#include "Process.h"
#include "SoftwareI2C.h"
#include <../LCD_display/Fonts/fonts.h>

extern volatile SpeedSensorData *speedSensorData;

class SSD1306Software : public SSD1306, public Process
{
private:
    /* data */
    SoftwareI2C i2cBus;
    std::ostringstream roundedValue;
    double lastRoundedValue;
    double currentRoundedValue;

    void init() override;
    void sendCommand(uint8_t) override;
    void display() override;

public:
    SSD1306Software();
    ~SSD1306Software();

    void loop() override;
};