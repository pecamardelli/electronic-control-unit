#include "TempGauge.h"

TempGauge::TempGauge(/* args */)
{
    motor.setSpeed(2);
}

TempGauge::~TempGauge()
{
    goToStartPosition();
}

void TempGauge::loop(float temp)
{
    // motor.step(100);
    // goToStartPosition();

    if (temp == 0)
    {
        while (currentStep > 0)
        {
            motor.step(-1);
            currentStep--;
            usleep(loopInterval);
        }
    }
    else
    {
        while (currentStep < temp)
        {
            motor.step(1);
            currentStep++;
            usleep(loopInterval);
        }
    }
}

void TempGauge::setup()
{
    logger.info("Setting up...");
    button.check();

    // Step forward until button is released.
    while (button.pressed)
    {
        motor.step(1);
        usleep(loopInterval);
        button.check();
    }

    // Get back to the initial position.
    goToStartPosition();

    motor.stop();
    logger.info("Temp Gauge ready!");
}

void TempGauge::goToStartPosition()
{
    while (!button.pressed)
    {
        motor.step(-1);
        usleep(loopInterval);
        button.check();
    }

    currentStep = 0;
}