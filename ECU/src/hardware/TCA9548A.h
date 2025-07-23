#pragma once

#include <iostream>
#include <memory>

#include "Logger.h"
#include <bcm2835.h>

#define TCA9548A_ADDRESS 0x70 // Default I2C address for the TCA9548A

class TCA9548A
{
private:
    /* data */
    std::unique_ptr<Logger> logger;
    std::string description = "TCA9548A";
    uint8_t currentChannel = 0;
    void reset();

public:
    TCA9548A(/* args */);
    ~TCA9548A();
    void selectChannel(uint8_t);
    uint8_t getCurrentChannel();
};
