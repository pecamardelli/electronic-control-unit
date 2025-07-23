#include "SpeedSensor.h"
#include <algorithm> // for std::clamp
#include <stdio.h>   // for printf

SpeedSensor::SpeedSensor(uint pin, const TireSpecs &specs, double gearRatio, double demultiplication)
    : sensorPin(pin), tireSpecs(specs), gearRatio(gearRatio), demultiplication(demultiplication), transitionsPerDriveshaftRev(SpeedSensorConfig::TRANSITIONS_PER_DRIVESHAFT_REV), lastTransitionDuration(0), elapsedTimeSinceLastTransition(0), lastDebounceTime(0), currentState(false), lastState(false), debouncedState(false), lastTime(0), currentTime(0), testEnabled(false), speedModifier(0.01), testSpeed(0.0)
{
    // Calculate derived values
    tireCircumference = calculateTireCircumference();
    kilometersPerTransition = 1.0 * demultiplication / transitionsPerDriveshaftRev / gearRatio * tireCircumference / 1000.0;

    // Initialize GPIO
    gpio_init(sensorPin);
    gpio_set_dir(sensorPin, GPIO_IN);
    gpio_pull_up(sensorPin);

    // Initialize sensor data
    reset();
}

SpeedSensor::~SpeedSensor()
{
    // Nothing to clean up
}

void SpeedSensor::reset()
{
    speedSensorData = {0, 0.0, 0.0, 0.0};
    lastTransitionDuration = 0;
    elapsedTimeSinceLastTransition = 0;
    lastTime = 0;
    currentTime = 0;
    lastState = false;
    currentState = false;
    debouncedState = false;
}

void SpeedSensor::setTestMode(bool enable, double initialSpeed)
{
    testEnabled = enable;
    if (enable)
    {
        testSpeed = std::clamp(initialSpeed,
                               SpeedSensorConfig::MIN_SPEED,
                               SpeedSensorConfig::MAX_SPEED);
        speedSensorData.speed = testSpeed;
    }
    else
    {
        reset();
    }
}

SpeedSensorData SpeedSensor::loop()
{
    currentTime = time_us_64();

    if (testEnabled)
    {
        updateTestMode();
    }
    else
    {
        updateRealMode();
    }

    return speedSensorData;
}

void SpeedSensor::updateTestMode()
{
    // Simulate speed changes
    testSpeed += speedModifier;

    // Reverse direction at speed limits
    if (testSpeed <= SpeedSensorConfig::MIN_SPEED ||
        testSpeed >= SpeedSensorConfig::MAX_SPEED)
    {
        speedModifier *= -1;
    }

    // Update sensor data with speed only, no distance
    speedSensorData.speed = testSpeed;
}

void SpeedSensor::updateRealMode()
{
    // Read and debounce sensor input
    currentState = gpio_get(sensorPin);
    debounceInput();

    // Detect falling edge (transition from 1 to 0)
    if (lastState && !debouncedState)
    {
        speedSensorData.transitions++;

        if (lastTime != 0)
        {
            lastTransitionDuration = currentTime - lastTime;
            speedSensorData.speed = calculateSpeed(lastTransitionDuration);
        }

        speedSensorData.distanceCovered = kilometersPerTransition * speedSensorData.transitions;
        lastTime = currentTime;
    }

    // Update speed based on time since last transition
    if (lastTime != 0)
    {
        elapsedTimeSinceLastTransition = currentTime - lastTime;

        // Gradually reduce speed if no new transitions
        if (elapsedTimeSinceLastTransition > lastTransitionDuration)
        {
            speedSensorData.speed = calculateSpeed(elapsedTimeSinceLastTransition);
        }

        // Consider car stopped after threshold
        if (elapsedTimeSinceLastTransition > SpeedSensorConfig::CAR_STOPPED_INTERVAL * 1e6)
        {
            speedSensorData.speed = 0;
        }
    }
    else
    {
        speedSensorData.speed = 0;
    }

    lastState = debouncedState;
}

void SpeedSensor::debounceInput()
{
    if (currentState != debouncedState)
    {
        if (currentTime - lastDebounceTime > SpeedSensorConfig::DEBOUNCE_TIME_US)
        {
            debouncedState = currentState;
        }
    }
    else
    {
        lastDebounceTime = currentTime;
    }
}

double SpeedSensor::calculateTireCircumference() const
{
    // Calculate tire diameter in mm: (width * aspect_ratio / 100 * 2) + (rim_diameter * 25.4)
    double diameter = (tireSpecs.width * tireSpecs.aspectRatio / 100.0 * 2) +
                      (tireSpecs.rimDiameter * 25.4);

    // Calculate circumference in mm: π * diameter
    return std::numbers::pi * diameter;
}

double SpeedSensor::calculateSpeed(uint64_t elapsedTime) const
{
    if (elapsedTime == 0)
    {
        return 0.0;
    }

    // Calculate driveshaft revolutions per second
    double revsPerSecond = 1e6 / elapsedTime / transitionsPerDriveshaftRev * demultiplication;

    // Calculate wheel revolutions per second
    double wheelRevsPerSecond = revsPerSecond / gearRatio;

    // Calculate speed in meters per second
    double metersPerSecond = wheelRevsPerSecond * tireCircumference;

    // Convert to kilometers per hour and validate
    double speed = metersPerSecond * 3.6;
    return std::clamp(speed, SpeedSensorConfig::MIN_SPEED, SpeedSensorConfig::MAX_SPEED);
}

bool SpeedSensor::isSpeedValid(double speed) const
{
    return speed >= SpeedSensorConfig::MIN_SPEED &&
           speed <= SpeedSensorConfig::MAX_SPEED;
}