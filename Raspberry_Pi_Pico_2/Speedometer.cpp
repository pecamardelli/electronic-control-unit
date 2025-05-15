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
        {20, 101},   // 20 km/h = 101 steps
        {30, 176},   // 30 km/h = 176 steps
        {40, 246},   // 40 km/h = 246 steps
        {50, 315},   // 50 km/h = 315 steps
        {60, 382},   // 60 km/h = 382 steps
        {70, 452},   // 70 km/h = 452 steps
        {80, 522},   // 80 km/h = 522 steps
        {90, 599},   // 90 km/h = 599 steps
        {100, 673},  // 100 km/h = 673 steps
        {110, 744},  // 110 km/h = 744 steps
        {120, 800},  // 120 km/h = 800 steps
        {130, 877},  // 130 km/h = 877 steps
        {140, 957},  // 140 km/h = 957 steps
        {150, 1033}, // 150 km/h = 1033 steps
        {160, 1108}, // 160 km/h = 1108 steps
        {170, 1181}, // 170 km/h = 1181 steps
        {180, 1255}, // 180 km/h = 1255 steps
        {190, 1326}, // 190 km/h = 1326 steps
        {200, 1398}, // 200 km/h = 1398 steps
        {210, 1464}, // 210 km/h = 1464 steps
        {220, 1539}, // 220 km/h = 1539 steps
        {230, 1610}, // 230 km/h = 1610 steps
        {240, 1687}  // 240 km/h = 1687 steps
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
    stepToGo = convertToStep(speed);

    // Calculate step difference for speed adjustment
    int stepDifference = std::abs(stepToGo - currentStep);

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
    if (currentStep < stepToGo)
    {
        currentStep++;
        motor->step(1);
    }
    else if (currentStep > stepToGo)
    {
        currentStep--;
        motor->step(-1);
    }
}
