#pragma once

#include <memory>
#include "Logger.h"
#include "../lib/ADS1115/ADS1115.h"

class VoltSensor
{
private:
    /* data */
    std::string description;
    std::unique_ptr<Logger> logger;
    const u_int8_t VOLT_SENSOR_CHANNEL = 0;
    ADS1115 *ads1115;

public:
    VoltSensor(ADS1115 *);
    ~VoltSensor();
    float getValue();
};