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
    uint16_t stepToGo = tempToStep(temp);

    if (stepToGo == currentStep)
        return;

    // std::cout << "Step to go: " << stepToGo << std::endl;

    if (currentStep < stepToGo)
    {
        while (currentStep < stepToGo)
        {
            motor.step(1);
            currentStep++;
            usleep(loopInterval);
        }
    }
    else
    {
        while (currentStep > stepToGo)
        {
            motor.step(-1);
            currentStep--;
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

uint16_t TempGauge::tempToStep(float temp)
{
    // Find the proper map object.
    auto conversion = std::find_if(conversions.begin(), conversions.end(), [temp](const Conversion &c)
                                   { return temp <= c.temp; });

    // Calculate the index using std::distance
    size_t index = std::distance(conversions.begin(), conversion);

    if (index == 0)
    {
        return 0;
    }

    auto lowerConversion = conversions.at(index - 1);

    float tempDiff = conversion->temp - lowerConversion.temp;
    uint16_t stepsDiff = conversion->step - lowerConversion.step;
    uint16_t stepToGo = lowerConversion.step + (temp - lowerConversion.temp) / tempDiff * stepsDiff;

    return stepToGo;
}