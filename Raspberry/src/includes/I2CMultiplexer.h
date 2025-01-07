#pragma once

#include <iostream>
#include <memory>

#include "BCM2835Manager.h"
#include "Logger.h"

#define TCA9548A_ADDRESS 0x70 // Default I2C address for the TCA9548A

class I2CMultiplexer
{
private:
    /* data */
    std::unique_ptr<Logger> logger;
    std::string description = "I2CMultiplexer";

public:
    I2CMultiplexer(/* args */);
    ~I2CMultiplexer();
    void selectChannel(uint8_t);
};
