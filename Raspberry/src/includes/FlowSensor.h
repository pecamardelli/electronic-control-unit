#pragma once

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Logger.h"
#include "System.h"

#define FLOW_SENSOR_PIN RPI_GPIO_P1_12 // GPIO18 (Pin 12)

typedef struct _flowSensorData
{
    unsigned long partialPulseCount;
    unsigned long totalPulseCount;
    float totalConsumption;
} FlowSensorData;

extern System sys;

class FlowSensor
{
private:
    /* data */
    useconds_t loopInterval;
    u_int16_t PULSES_PER_LITER;
    uint8_t lastState;
    uint8_t currentState;
    Logger logger = Logger("FlowSensor");

public:
    FlowSensor(/* args */);
    ~FlowSensor();
    FlowSensorData data = {0};
    const std::string description = "FlowSensor";

    FlowSensorData loop();
};