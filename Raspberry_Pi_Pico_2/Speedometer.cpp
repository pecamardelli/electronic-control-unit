#include "Speedometer.h"

Speedometer::Speedometer(/* args */)
{

    motor = std::make_unique<Stepper>(2038, IN1, IN3, IN2, IN4);
    interruptor = std::make_unique<Button>(INTERRUPTOR);

    conversions.push_back({0, 0});
    conversions.push_back({20, 101});
    conversions.push_back({30, 176});
    conversions.push_back({40, 246});
    conversions.push_back({50, 315});
    conversions.push_back({60, 382});
    conversions.push_back({70, 452});
    conversions.push_back({80, 522});
    conversions.push_back({90, 599});
    conversions.push_back({100, 673});
    conversions.push_back({110, 744});
    conversions.push_back({120, 800});
    conversions.push_back({130, 877});
    conversions.push_back({140, 957});
    conversions.push_back({150, 1033});
    conversions.push_back({160, 1108});
    conversions.push_back({170, 1181});
    conversions.push_back({180, 1255});
    conversions.push_back({190, 1326});
    conversions.push_back({200, 1398});
    conversions.push_back({210, 1464});
    conversions.push_back({220, 1539});
    conversions.push_back({230, 1610});
    conversions.push_back({240, 1687});
    init();

    // test(3, 5);

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
