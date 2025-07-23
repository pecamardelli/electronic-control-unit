#pragma once

#include "Logger.h"
#include <bcm2835.h>

#define RELAY_PIN RPI_GPIO_P1_11 // Physical Pin 11, corresponds to GPIO17

class RELAY
{
private:
    /* data */
    Logger logger = Logger("RELAY");

public:
    RELAY(/* args */);
    ~RELAY();
    void state(int);
};