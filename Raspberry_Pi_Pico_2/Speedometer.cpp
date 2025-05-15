/**
 * @file Speedometer.cpp
 * @brief Implementation of the Speedometer class for controlling a stepper motor-based speed gauge
 *
 * This file implements the functionality for controlling a stepper motor that drives
 * a mechanical speedometer needle. It includes speed-to-step conversion and dynamic
 * motor speed adjustment based on the distance to the target position.
 */

#include "Speedometer.h"

/**
 * @namespace SpeedometerConfig
 * @brief Configuration settings for the speedometer
 *
 * This namespace contains all the configuration constants used by the Speedometer class,
 * including speed limits, thresholds for motor speed adjustment, and motor speed settings.
 */
namespace SpeedometerConfig
{
    constexpr int MIN_SPEED = 0;               ///< Minimum valid speed in km/h
    constexpr int MAX_SPEED = 240;             ///< Maximum valid speed in km/h
    constexpr int SPEED_THRESHOLD_FAST = 100;  ///< Threshold for fast motor movement
    constexpr int SPEED_THRESHOLD_MEDIUM = 50; ///< Threshold for medium motor movement
    constexpr int SPEED_THRESHOLD_SLOW = 10;   ///< Threshold for slow motor movement
    constexpr int MOTOR_SPEED_FAST = 4;        ///< Motor speed for large adjustments
    constexpr int MOTOR_SPEED_MEDIUM = 3;      ///< Motor speed for medium adjustments
    constexpr int MOTOR_SPEED_SLOW = 2;        ///< Motor speed for small adjustments
    constexpr int MOTOR_SPEED_FINE = 1;        ///< Motor speed for fine adjustments
}

/**
 * @brief Constructs a new Speedometer object
 *
 * Initializes the stepper motor with 2038 steps per revolution and sets up the
 * speed-to-step conversion table. The conversion table maps speed values to
 * corresponding step positions for the motor.
 */
Speedometer::Speedometer()
{
    // Initialize stepper motor with 2038 steps per revolution
    motor = std::make_unique<Stepper>(2038, IN1, IN3, IN2, IN4);
    interruptor = std::make_unique<Button>(INTERRUPTOR);

    // Initialize speed-to-step conversion table
    // Format: {speed (km/h), steps}
    conversions = {
        {0, 0},      // 0 km/h = 0 steps
        {20, 97},    // 20 km/h = 97 steps
        {30, 174},   // 30 km/h = 174 steps
        {40, 246},   // 40 km/h = 246 steps
        {50, 315},   // 50 km/h = 315 steps
        {60, 392},   // 60 km/h = 392 steps
        {70, 458},   // 70 km/h = 458 steps
        {80, 528},   // 80 km/h = 528 steps
        {90, 605},   // 90 km/h = 605 steps
        {100, 676},  // 100 km/h = 676 steps
        {110, 744},  // 110 km/h = 744 steps
        {120, 820},  // 120 km/h = 820 steps
        {130, 893},  // 130 km/h = 893 steps
        {140, 962},  // 140 km/h = 962 steps
        {150, 1033}, // 150 km/h = 1033 steps
        {160, 1108}, // 160 km/h = 1108 steps
        {170, 1178}, // 170 km/h = 1178 steps
        {180, 1252}, // 180 km/h = 1252 steps
        {190, 1323}, // 190 km/h = 1323 steps
        {200, 1392}, // 200 km/h = 1392 steps
        {210, 1461}, // 210 km/h = 1461 steps
        {220, 1536}, // 220 km/h = 1536 steps
        {230, 1607}, // 230 km/h = 1607 steps
        {240, 1680}  // 240 km/h = 1680 steps
    };

    init();
    motor->setSpeed(SpeedometerConfig::MOTOR_SPEED_FINE);
}

/**
 * @brief Destructor for the Speedometer object
 *
 * Ensures the motor is properly stopped when the Speedometer object is destroyed.
 */
Speedometer::~Speedometer()
{
    if (motor)
    {
        motor->stop();
    }
}

/**
 * @brief Main control loop for the speedometer
 *
 * This method is called repeatedly to update the speedometer position based on
 * the current speed. It handles:
 * 1. Speed validation and clamping
 * 2. Conversion of speed to step position
 * 3. Dynamic motor speed adjustment
 * 4. Motor movement control
 *
 * @param speed The current speed in kilometers per hour
 */
void Speedometer::loop(double speed)
{
    // Clamp speed to valid range
    speed = std::clamp(speed,
                       static_cast<double>(SpeedometerConfig::MIN_SPEED),
                       static_cast<double>(SpeedometerConfig::MAX_SPEED));

    // Calculate target step position
    int newStepToGo = convertToStep(speed);

    // Add mechanical play compensation when changing direction
    static int lastDirection = 0; // 0: no movement, 1: increasing, -1: decreasing
    int currentDirection = newStepToGo > currentStep ? 1 : (newStepToGo < currentStep ? -1 : 0);

    if (currentDirection != 0 && currentDirection != lastDirection)
    {
        // Add 4 steps of compensation when changing direction
        newStepToGo += currentDirection * 15;
    }
    lastDirection = currentDirection;

    // Calculate step difference for speed adjustment
    int stepDifference = std::abs(newStepToGo - currentStep);

    // Adjust motor speed based on distance to target
    if (stepDifference > SpeedometerConfig::SPEED_THRESHOLD_FAST)
    {
        motor->setSpeed(SpeedometerConfig::MOTOR_SPEED_FAST);
    }
    else if (stepDifference > SpeedometerConfig::SPEED_THRESHOLD_MEDIUM)
    {
        motor->setSpeed(SpeedometerConfig::MOTOR_SPEED_MEDIUM);
    }
    else if (stepDifference > SpeedometerConfig::SPEED_THRESHOLD_SLOW)
    {
        motor->setSpeed(SpeedometerConfig::MOTOR_SPEED_SLOW);
    }
    else
    {
        motor->setSpeed(SpeedometerConfig::MOTOR_SPEED_FINE);
    }

    // Move motor one step in the appropriate direction
    if (currentStep < newStepToGo)
    {
        motor->step(1);
        currentStep++;
    }
    else if (currentStep > newStepToGo)
    {
        motor->step(-1);
        currentStep--;
    }

    // Update stepToGo for next iteration
    stepToGo = newStepToGo;
}
