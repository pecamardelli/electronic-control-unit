#pragma once

#include "BCM2835Manager.h"

class Button
{
private:
    /* data */
    RPiGPIOPin pin;

public:
    Button(RPiGPIOPin);
    ~Button();
    void check();
    bool pressed;
};