#pragma once

#include "OF05ZAT.h"
#include "Process.h"
#include <bcm2835.h>
#include <chrono>
#include <thread>

#define MAIN_SENSOR_PIN RPI_V2_GPIO_P1_38
#define RETURN_SENSOR_PIN RPI_V2_GPIO_P1_40

extern volatile FuelConsumptionData *fuelConsumptionData;

class FuelConsumption : public Process
{
private:
    /* data */
    OF05ZAT mainSensor;
    OF05ZAT returnSensor;

    uint16_t mainSensorPulsesPerLiter = 0;
    uint16_t returnSensorPulsesPerLiter = 0;
    uint64_t mainPulseCounter = 0;
    uint64_t returnPulseCounter = 0;

    bool testEnabled = false;
    uint64_t testInterval = 0;

public:
    FuelConsumption(/* args */);
    ~FuelConsumption();
    void loop() override;
};
