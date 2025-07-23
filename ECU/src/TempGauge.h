/*
    STEP TO TEMPERATURE MAPPING
    40°  -> 45 steps
    60°  -> 105 steps
    80°  -> 188 steps
    100° -> 278 steps
    130° -> 354 steps
*/

#pragma once

#include "Process.h"
#include "Gauge.h"
#include "Logger.h"
#include "Button.h"

#include "STEPPER.h"

extern volatile CoolantTempSensorData *coolantTempSensorData;
class TempGauge : public Process, public Gauge
{
private:
    // Define GPIO pins for the ULN2003AN
    const RPiGPIOPin IN1 = RPI_V2_GPIO_P1_18;         // GPIO 24
    const RPiGPIOPin IN2 = RPI_V2_GPIO_P1_26;         // GPIO 7
    const RPiGPIOPin IN3 = RPI_V2_GPIO_P1_15;         // GPIO 22
    const RPiGPIOPin IN4 = RPI_V2_GPIO_P1_21;         // GPIO 9
    const RPiGPIOPin INTERRUPTOR = RPI_V2_GPIO_P1_12; // GPIO 18
    int stepToGo = 0;
    float currentTemp = 0;

public:
    TempGauge(/* args */);
    ~TempGauge();

    void loop() override;
};