#include "SpeedSensor.h"

SpeedSensor::SpeedSensor(/* args */)
{
    tireCircumference = calculateTireCircumference();
    kilometersPerTransition = 1.0 * demultiplication / 4.0 / gearRatio * tireCircumference / 1000.0;

    if (lowestSpeed < 0)
        lowestSpeed = 0;
    if (highestSpeed > 240)
        highestSpeed = 240;
    if (lowestSpeed > highestSpeed)
    {
        lowestSpeed = 0;
        highestSpeed = 240;
    }

    // Initialize the shared data
    speedSensorData.transitions = 0;
    speedSensorData.speed = testEnabled ? lowestSpeed : 0.0;
    speedSensorData.distanceCovered = 0.0;
    speedSensorData.averageSpeed = 0.0;

    gpio_init(D0_PIN);             // Initialize the pin
    gpio_set_dir(D0_PIN, GPIO_IN); // Set as input
    gpio_pull_up(D0_PIN);          // Enable pull-up resistor
}

SpeedSensor::~SpeedSensor()
{
}

SpeedSensorData SpeedSensor::loop()
{
    // Read the current time in microseconds
    currentTime = time_us_64();

    // Simulation mode for testing
    if (testEnabled)
    {
        if (speedSensorData.speed < lowestSpeed || speedSensorData.speed > highestSpeed)
            speedModifier *= -1;

        speedSensorData.speed = speedSensorData.speed + speedModifier;
        speedSensorData.distanceCovered = speedSensorData.distanceCovered + 0.01;
    }
    else
    {
        // Real sensor mode
        // Read the current state of the digital output (D0)
        currentState = gpio_get(D0_PIN);

        // Detect a transition from 1 to 0 (object detection edge)
        if (lastState && !currentState)
        {
            speedSensorData.transitions++;

            if (lastTime != 0)
            {
                // Calculate speed based on the time difference since the last transition
                lastTransitionDuration = currentTime - lastTime;
                speedSensorData.speed = calculateSpeed(lastTransitionDuration);
            }

            speedSensorData.distanceCovered = kilometersPerTransition * speedSensorData.transitions;

            // Update the last detection time
            lastTime = currentTime;
        }

        // If there's been at least one transition, estimate speed
        if (lastTime != 0)
        {
            // Calculate the time elapsed since the last transition
            elapsedTimeSinceLastTransition = currentTime - lastTime;

            // If no transition has occurred for a while, gradually reduce the speed to zero
            if (elapsedTimeSinceLastTransition > lastTransitionDuration)
            {
                speedSensorData.speed = calculateSpeed(elapsedTimeSinceLastTransition);
            }

            // If the car is considered stopped, set the speed to 0
            if (elapsedTimeSinceLastTransition > carStoppedInterval * 1e6)
            {
                speedSensorData.speed = 0;
            }
        }
        else
        {
            // No transitions detected yet, so set speed to 0
            speedSensorData.speed = 0;
        }

        // Update the last state
        lastState = currentState;
    }

    return speedSensorData;
}

double SpeedSensor::calculateTireCircumference()
{
    // Tire circumference in meters
    return std::numbers::pi * (tireWidth * aspectRatio / 100.0 * 2 + rimDiameter * 25.4) / 1000.0;
}

double SpeedSensor::calculateSpeed(uint64_t elapsedTime)
{
    if (elapsedTime == 0)
    {
        return 0.0; // Avoid division by zero
    }

    driveshaftRevsPerSecond = 1e6 / elapsedTime / transitionsPerDriveshaftRev * demultiplication;

    // Wheel laps per second
    wheelRevsPerSecond = driveshaftRevsPerSecond / gearRatio;

    // Speed in meters per second
    metersPerSecond = wheelRevsPerSecond * tireCircumference;

    // Convert to kilometers per hour
    return metersPerSecond * 3.6;
}