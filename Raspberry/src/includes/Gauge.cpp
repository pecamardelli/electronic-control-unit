#include "Gauge.h"

Gauge::Gauge(/* args */)
{
}

Gauge::~Gauge()
{
}

void Gauge::init()
{
    motor->setSpeed(3);
    interruptor->check();

    // Step forward until button is released.
    while (interruptor->pressed)
    {
        motor->step(1);
        interruptor->check();
    }

    // Get back to the initial position.
    goToStartPosition();
    motor->setSpeed(1);
}

void Gauge::goToStartPosition()
{
    if (motor == nullptr || interruptor == nullptr)
        return;

    interruptor->check();

    while (!interruptor->pressed)
    {
        motor->step(-1);
        interruptor->check();
    }

    // Add offset to place the gauge needle correctly.
    motor->step(stepOffset);
    motor->stop();

    currentStep = 0;
}

int Gauge::convertToStep(float value)
{
    // Find the proper map object.
    auto conversion = std::find_if(conversions.begin(), conversions.end(), [value](const Conversion &c)
                                   { return value <= c.value; });

    // Calculate the index using std::distance
    size_t index = std::distance(conversions.begin(), conversion);

    if (index == 0)
    {
        return 0;
    }

    auto lowerConversion = conversions.at(index - 1);

    float valueDiff = conversion->value - lowerConversion.value;
    uint16_t stepsDiff = conversion->step - lowerConversion.step;
    uint16_t stepToGo = lowerConversion.step + (value - lowerConversion.value) / valueDiff * stepsDiff;

    return stepToGo;
}

void Gauge::test(long motorSpeed, unsigned int testInterval)
{
    int previousStep = 0;
    motor->setSpeed(motorSpeed);

    for (const auto &conversion : conversions)
    {
        motor->step(conversion.step - previousStep);
        previousStep = conversion.step;
        sleep(testInterval);
    }

    goToStartPosition();
}

void Gauge::calibrate()
{
    int number = 0;
    std::cout << "Entering calibration mode." << std::endl;
    std::cout << "Enter 0 to go to the start position and a non integer value to exit." << std::endl;

    motor->setSpeed(3);

    while (true)
    {
        std::cout << "Enter a number to move the gauge needle (current step: " << currentStep << "): ";
        std::cin >> number;

        if (std::cin.fail())
        {
            std::cout << "Exiting calibration mode." << std::endl;
            currentStep = 0;
            break;
        }
        else if (number == 0)
        {
            goToStartPosition();
        }
        else
        {
            motor->step(number);
            currentStep += number;
        }
    }

    goToStartPosition();
}