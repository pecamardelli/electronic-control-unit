#pragma once

#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>

#define RELAY_PIN RPI_GPIO_P1_11 // Physical Pin 11, corresponds to GPIO17

class Relay
{
private:
    /* data */
public:
    Relay(/* args */);
    ~Relay();
    void state(int);
};