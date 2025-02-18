#pragma once

// #define RESET_PIN RPI_V2_GPIO_P1_35 // GPIO19

#include <iostream>
#include <memory>

#include "Logger.h"
#include "BCM_2835.h"

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
