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
#include "Logger.h"
#include "System.h"
#include "Button.h"

#include "lib/Stepper/Stepper.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
struct Conversion
{
    float temp;
    uint16_t step;
};

extern System *sys;
class TempGauge : public Process
{
private:
    // Define GPIO pins for the ULN2003AN
    const RPiGPIOPin IN1 = RPI_V2_GPIO_P1_36; // GPIO 16
    const RPiGPIOPin IN2 = RPI_V2_GPIO_P1_38; // GPIO 20
    const RPiGPIOPin IN3 = RPI_V2_GPIO_P1_40; // GPIO 21
    const RPiGPIOPin IN4 = RPI_V2_GPIO_P1_37; // GPIO 26

    // GPIO pin connected to the interruptor switch
    const RPiGPIOPin BUTTON_PIN = RPI_V2_GPIO_P1_32; // GPIO 12 (Pin 32)

    std::vector<Conversion>
        conversions = {
            {30, 0}, {40, 45}, {60, 100}, {80, 185}, {100, 275}, {130, 351}};

    Logger logger = Logger("TempGauge");
    Stepper motor = Stepper(2038, IN1, IN3, IN2, IN4);
    Button button = Button(BUTTON_PIN);
    int currentStep = -1;
    int stepOffset = 0;

    void goToStartPosition();
    uint16_t tempToStep(float);

public:
    TempGauge(/* args */);
    ~TempGauge();

    void setup() override;
    void loop() override;
};