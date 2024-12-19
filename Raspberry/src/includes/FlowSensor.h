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
    const std::string description = "FlowSensor";
    const useconds_t loopInterval = sys.getConfigValue<useconds_t>(description, "loop_interval");
    const u_int16_t PULSES_PER_LITER = sys.getConfigValue<u_int16_t>(description, "pulses_per_liter");
    uint8_t lastState;
    uint8_t currentState;
    Logger logger = Logger("FlowSensor");

public:
    FlowSensor(/* args */);
    ~FlowSensor();
    FlowSensorData data = {0};

    FlowSensorData loop();
};