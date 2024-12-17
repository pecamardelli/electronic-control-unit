#include "TempGauge.h"

TempGauge::TempGauge(/* args */)
{
    motor.setSpeed(1);
}

TempGauge::~TempGauge()
{
}

void TempGauge::loop()
{
    button.check();

    if (button.pressed)
    {
        direction = 1;
    }

    motor.step(1);
    steps += 1;

    usleep(10000);
}