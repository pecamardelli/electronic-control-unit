#pragma once

#include "Logger.h"
#include "BCM2835Manager.h"

#define RELAY_PIN RPI_GPIO_P1_11 // Physical Pin 11, corresponds to GPIO17

class Relay
{
private:
    /* data */
    Logger logger = Logger("Relay");

public:
    Relay(/* args */);
    ~Relay();
    void state(int);
};