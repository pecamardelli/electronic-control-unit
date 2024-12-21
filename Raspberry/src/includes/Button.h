#pragma once

#include <bcm2835.h>
#include <iostream>
#include "Logger.h"

class Button
{
private:
    /* data */
    Logger logger = Logger("Button");
    RPiGPIOPin pin;

public:
    Button(RPiGPIOPin);
    ~Button();
    void check();
    bool pressed;
};