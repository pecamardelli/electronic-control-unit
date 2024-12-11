#ifndef FLOW_SENSOR_H_
#define FLOW_SENSOR_H_

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define FLOW_SENSOR_PIN RPI_GPIO_P1_12 // GPIO18 (Pin 12)
#define PULSES_PER_LITER 287           // Example value, check your datasheet

class FlowSensor
{
private:
    /* data */
    uint8_t last_state;
    uint8_t current_state;

public:
    FlowSensor(/* args */);
    ~FlowSensor();
    unsigned long pulse_count = 0;

    void checkPulses();
};

#endif