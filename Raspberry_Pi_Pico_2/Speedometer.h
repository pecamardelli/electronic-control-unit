#pragma once

#include <cstring>

#include "Gauge.h"

class Speedometer : public Gauge
{
private:
    /* data */
    const uint IN1 = 21;
    const uint IN2 = 20;
    const uint IN3 = 19;
    const uint IN4 = 18;
    const uint INTERRUPTOR = 27;

    uint64_t lastTotalMileage = 0;
    double speed = 0;
    double lastSpeed = 0;
    int stepToGo = 0;
    int currentStep = 0;

public:
    Speedometer(/* args */);
    ~Speedometer();
    void loop();
};