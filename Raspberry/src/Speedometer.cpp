#include "Speedometer.h"

Speedometer::Speedometer(/* args */)
{
    description = "Speedometer";
    ssd1306 = new SSD1306();
}

Speedometer::~Speedometer()
{
}

void Speedometer::loop(EngineValues *engineValues)
{
    while (!terminateFlag.load())
        usleep(1000000);
}