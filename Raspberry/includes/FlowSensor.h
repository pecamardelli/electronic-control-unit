#ifndef FLOW_SENSOR_H_
#define FLOW_SENSOR_H_

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Logger.h"

#define FLOW_SENSOR_PIN RPI_GPIO_P1_12 // GPIO18 (Pin 12)

typedef struct _flowSensorData
{
    unsigned long partialPulseCount;
    unsigned long totalPulseCount;
    float totalConsumption;
} FlowSensorData;

class FlowSensor : Logger
{
private:
    /* data */
    uint8_t lastState;
    uint8_t currentState;
    const u_int16_t PULSES_PER_LITER = 1;

public:
    FlowSensor(/* args */);
    ~FlowSensor();
    FlowSensorData data = {0};

    FlowSensorData loop();
};

#endif