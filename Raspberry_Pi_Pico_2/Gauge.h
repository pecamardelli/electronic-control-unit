#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <sys/unistd.h>
#include <iostream>

#include "Stepper.h"
#include "Button.h"

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
    void calibrate();

protected:
    std::unique_ptr<Stepper> motor;
    std::unique_ptr<Button> interruptor;
    std::vector<Conversion> conversions;
    int currentStep = 0;
    int stepOffset = 0;

    void init();
    void goToStartPosition();
    int convertToStep(double);
    void test(long, unsigned int);
};