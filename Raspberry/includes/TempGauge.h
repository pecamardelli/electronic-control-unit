/*
    STEP TO TEMPERATURE MAPPING
    40°  -> 45 steps
    60°  -> 105 steps
    80°  -> 188 steps
    100° -> 278 steps
    130° -> 354 steps
*/

#ifndef TEMP_GAUGE_H_
#define TEMP_GAUGE_H_

#include "Logger.h"
#include "../lib/Stepper/Stepper.h"
#include "Button.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
struct Conversion
{
    float temp;
    uint16_t step;
};

class TempGauge
{
private:
    // Define GPIO pins for the ULN2003AN
    const uint8_t IN1 = RPI_V2_GPIO_P1_36; // GPIO 16
    const uint8_t IN2 = RPI_V2_GPIO_P1_38; // GPIO 20
    const uint8_t IN3 = RPI_V2_GPIO_P1_40; // GPIO 21
    const uint8_t IN4 = RPI_V2_GPIO_P1_37; // GPIO 26
    const useconds_t loopInterval = 20000;

    std::vector<Conversion> conversions = {
        {30, 0}, {40, 45}, {60, 100}, {80, 185}, {100, 275}, {130, 351}};

    Logger logger = Logger("TempGauge");
    Stepper motor = Stepper(2038, IN1, IN3, IN2, IN4);
    Button button;
    uint16_t currentStep = -1;

    void goToStartPosition();
    uint16_t tempToStep(float);

public:
    TempGauge(/* args */);
    ~TempGauge();

    void setup();
    void loop(float);
};

#endif