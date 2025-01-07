#pragma once

#include <cstring>

#include "Process.h"
#include "Gauge.h"
#include "lib/SSD1306/SSD1306.h"
class Speedometer : public Process, public Gauge
{
private:
    /* data */
    const RPiGPIOPin IN1 = RPI_V2_GPIO_P1_29;         // GPIO 5
    const RPiGPIOPin IN2 = RPI_V2_GPIO_P1_31;         // GPIO 6
    const RPiGPIOPin IN3 = RPI_V2_GPIO_P1_33;         // GPIO 13
    const RPiGPIOPin IN4 = RPI_V2_GPIO_P1_37;         // GPIO 26
    const RPiGPIOPin INTERRUPTOR = RPI_V2_GPIO_P1_07; // GPIO 18

    std::unique_ptr<SSD1306> upperDisplay;

public:
    Speedometer(/* args */);
    ~Speedometer();
    void loop(EngineValues *) override;
};