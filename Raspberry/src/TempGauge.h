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
#include "common.h"

#include "lib/Stepper/Stepper.h"

class TempGauge : public Process, public Gauge
{
private:
    // Define GPIO pins for the ULN2003AN
    const RPiGPIOPin IN1 = RPI_V2_GPIO_P1_32;         // GPIO 16
    const RPiGPIOPin IN2 = RPI_V2_GPIO_P1_36;         // GPIO 20
    const RPiGPIOPin IN3 = RPI_V2_GPIO_P1_38;         // GPIO 21
    const RPiGPIOPin IN4 = RPI_V2_GPIO_P1_40;         // GPIO 26
    const RPiGPIOPin INTERRUPTOR = RPI_V2_GPIO_P1_12; // GPIO 18

public:
    TempGauge(/* args */);
    ~TempGauge();

    void loop(EngineValues *) override;
};