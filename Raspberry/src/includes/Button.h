#pragma once

#include <bcm2835.h>

class Button
{
private:
    /* data */
    RPiGPIOPin pin;
    uint8_t buttonState;
    uint8_t stateCount = 0;

public:
    Button(RPiGPIOPin);
    ~Button();
    void check();
    bool pressed;
};