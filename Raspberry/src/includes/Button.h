#pragma once

#include "Logger.h"
#include "BCM2835Manager.h"

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