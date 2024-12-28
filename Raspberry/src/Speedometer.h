#pragma once

#include "Process.h"
#include "../lib/SSD1306/SSD1306.h"

class Speedometer : public Process
{
private:
    /* data */
    SSD1306 *ssd1306;

public:
    Speedometer(/* args */);
    ~Speedometer();
    void loop(EngineValues *) override;
};