#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "../lib/Stepper/Stepper.h"
#include "Button.h"
#include "Logger.h"

struct Conversion
{
    float value;
    int step;
};

class Gauge
{
private:
    /* data */

public:
    Gauge(/* args */);
    ~Gauge();

protected:
    std::unique_ptr<Stepper> motor;
    std::unique_ptr<Button> interruptor;
    std::vector<Conversion> conversions;
    int currentStep = 0;
    int stepOffset = 0;

    void init();
    void goToStartPosition();
    int convertToStep(float);
    void test(long, unsigned int);
};