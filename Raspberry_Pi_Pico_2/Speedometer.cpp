#include "Speedometer.h"

Speedometer::Speedometer(/* args */)
{

    motor = std::make_unique<Stepper>(2038, IN1, IN3, IN2, IN4);
    interruptor = std::make_unique<Button>(INTERRUPTOR);

    conversions.push_back({0, 0});
    conversions.push_back({20, 80});
    conversions.push_back({30, 155});
    conversions.push_back({40, 225});
    conversions.push_back({50, 297});
    conversions.push_back({60, 365});
    conversions.push_back({70, 435});
    conversions.push_back({80, 506});
    conversions.push_back({90, 577});
    conversions.push_back({100, 652});
    conversions.push_back({110, 722});
    conversions.push_back({120, 795});
    conversions.push_back({130, 870});
    conversions.push_back({140, 942});
    conversions.push_back({150, 1014});
    conversions.push_back({160, 1088});
    conversions.push_back({170, 1162});
    conversions.push_back({180, 1230});
    conversions.push_back({190, 1303});
    conversions.push_back({200, 1371});
    conversions.push_back({210, 1444});
    conversions.push_back({220, 1520});
    conversions.push_back({230, 1593});
    conversions.push_back({240, 1666});
    init();

    motor->setSpeed(1);
}

Speedometer::~Speedometer()
{
}

void Speedometer::loop()
{
    speed = 0;

    if (speed == lastSpeed)
    {
        motor->stop();
        return;
    }

    lastSpeed = speed;

    if (!speed || speed < 0)
        speed = 0;
    else if (speed > 240)
        speed = 240;

    stepToGo = convertToStep(speed);

    // Calculate the difference between the target and current step
    int stepDifference = std::abs(stepToGo - currentStep);

    // Dynamically adjust the motor speed based on step difference
    if (stepDifference > 100)
    {
        motor->setSpeed(4); // Fastest speed for large adjustments
    }
    else if (stepDifference > 50)
    {
        motor->setSpeed(3); // Medium-fast speed for moderate adjustments
    }
    else if (stepDifference > 10)
    {
        motor->setSpeed(2); // Medium speed for smaller adjustments
    }
    else
    {
        motor->setSpeed(1); // Slowest speed for fine adjustments
    }

    motor->step(stepToGo - currentStep);
    currentStep = stepToGo;

    motor->step(-currentStep);
}
