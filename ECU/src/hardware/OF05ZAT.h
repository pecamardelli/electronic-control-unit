#pragma once

#include "Base.h"

class OF05ZAT : public Base
{
private:
    /* data */
    RPiGPIOPin pin;
    uint16_t pulsesPerLiter;
    uint8_t lastState;
    uint8_t currentState;
    uint64_t pulseCounter;

    void checkPulse();

public:
    OF05ZAT(RPiGPIOPin);
    ~OF05ZAT();

    uint64_t getPulseCount();
    uint16_t getPulsesPerLiter();
};