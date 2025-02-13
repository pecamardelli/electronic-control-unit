#pragma once

#include <cstring>
#include <chrono>
#include <thread>

#include "Process.h"
#include "Gauge.h"
#include "lib/SSD1306/SSD1306.h"

extern volatile SpeedSensorData *speedSensorData;
class Speedometer : public Process, public Gauge
{
private:
    /* data */
    const RPiGPIOPin IN1 = RPI_V2_GPIO_P1_29;         // GPIO 5
    const RPiGPIOPin IN2 = RPI_V2_GPIO_P1_31;         // GPIO 6
    const RPiGPIOPin IN3 = RPI_V2_GPIO_P1_33;         // GPIO 13
    const RPiGPIOPin IN4 = RPI_V2_GPIO_P1_37;         // GPIO 26
    const RPiGPIOPin INTERRUPTOR = RPI_V2_GPIO_P1_07; // GPIO 18

    SSD1306 upperDisplay;

    uint64_t lastTotalMileage = 0;
    double speed = 0;
    double lastSpeed = 0;
    int stepToGo = 0;
    int currentStep = 0;

public:
    Speedometer(/* args */);
    ~Speedometer();
    void loop() override;
};