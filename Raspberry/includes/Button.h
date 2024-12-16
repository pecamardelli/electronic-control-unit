#ifndef BUTTON_H_
#define BUTTON_H_

#include <bcm2835.h>
#include <iostream>
#include "Logger.h"

// GPIO pin connected to the interruptor switch
#define BUTTON_PIN RPI_V2_GPIO_P1_32 // GPIO 12 (Pin 32)

class Button
{
private:
    /* data */
    Logger logger = Logger("Button");

public:
    Button(/* args */);
    ~Button();
    void check();
    volatile bool pressed;
};

#endif