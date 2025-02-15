#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Logger.h"
#include "Process.h"
#include "common.h"

#define FLOW_SENSOR_PIN RPI_GPIO_P1_12 // GPIO18 (Pin 12)

typedef struct _flowSensorData
{
    unsigned long partialPulseCount;
    unsigned long totalPulseCount;
    float totalConsumption;
} FlowSensorData;

class OF05ZAT : public Process
{
private:
    /* data */
    u_int16_t pulsesPerLiter;
    uint8_t lastState;
    uint8_t currentState;

public:
    OF05ZAT(/* args */);
    ~OF05ZAT();
    FlowSensorData data = {0};

    void loop() override;
};