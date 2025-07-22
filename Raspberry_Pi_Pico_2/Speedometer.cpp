/**
 * @file Speedometer.cpp
 * @brief Implementation of the Speedometer class for controlling a stepper motor-based speed gauge
 *
 * This file implements the functionality for controlling a stepper motor that drives
 * a mechanical speedometer needle. It includes speed-to-step conversion and dynamic
 * motor speed adjustment based on the distance to the target position.
 */

#include "Speedometer.h"
#include <cmath>
#include <algorithm>
#include <stdio.h>

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

    // Speed filtering configuration
    constexpr double DEAD_ZONE_THRESHOLD = 2.0; ///< Speeds below this are considered stationary (km/h)
    constexpr double HYSTERESIS_START = 3.0;    ///< Speed to start movement from stationary (km/h)
    constexpr double HYSTERESIS_STOP = 1.5;     ///< Speed to stop movement when moving (km/h)
    constexpr double MAX_SPEED_CHANGE = 15.0;   ///< Maximum allowed speed change per update (km/h)
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
 * @brief Filters the raw GPS speed to reduce noise and improve accuracy
 *
 * This method implements multiple filtering techniques to handle GPS speed noise:
 * 1. Dead zone: Speeds below threshold are treated as stationary
 * 2. Hysteresis: Different thresholds for starting/stopping movement
 * 3. Moving average: Smooths out rapid fluctuations
 * 4. Rate limiting: Prevents sudden speed jumps
 *
 * @param rawSpeed The raw speed from GPS in km/h
 * @return The filtered speed in km/h
 */
double Speedometer::filterSpeed(double rawSpeed)
{
    // Step 1: Apply dead zone for very low speeds
    if (rawSpeed < SpeedometerConfig::DEAD_ZONE_THRESHOLD)
    {
        rawSpeed = 0.0;
    }

    // Step 2: Apply hysteresis to prevent oscillation around threshold
    static bool isMoving = false;
    if (!isMoving && rawSpeed >= SpeedometerConfig::HYSTERESIS_START)
    {
        isMoving = true;
    }
    else if (isMoving && rawSpeed <= SpeedometerConfig::HYSTERESIS_STOP)
    {
        isMoving = false;
        rawSpeed = 0.0;
    }
    else if (!isMoving)
    {
        rawSpeed = 0.0;
    }

    // Step 3: Add to circular buffer for moving average
    speedBuffer[bufferIndex] = rawSpeed;
    bufferIndex = (bufferIndex + 1) % FILTER_BUFFER_SIZE;
    if (!bufferFilled && bufferIndex == 0)
    {
        bufferFilled = true;
    }

    // Step 4: Calculate moving average
    double sum = 0.0;
    int count = bufferFilled ? FILTER_BUFFER_SIZE : bufferIndex;
    for (int i = 0; i < count; i++)
    {
        sum += speedBuffer[i];
    }
    double averagedSpeed = count > 0 ? sum / count : 0.0;

    // Step 5: Apply rate limiting to prevent sudden jumps
    double speedDifference = averagedSpeed - filteredSpeed;
    if (std::abs(speedDifference) > SpeedometerConfig::MAX_SPEED_CHANGE)
    {
        // Limit the rate of change
        if (speedDifference > 0)
        {
            averagedSpeed = filteredSpeed + SpeedometerConfig::MAX_SPEED_CHANGE;
        }
        else
        {
            averagedSpeed = filteredSpeed - SpeedometerConfig::MAX_SPEED_CHANGE;
        }
    }

    return averagedSpeed;
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
    // Apply speed filtering to reduce GPS noise
    double originalSpeed = speed;
    filteredSpeed = filterSpeed(speed);

    // Debug output for speed filtering and backlash compensation
    static int debugCounter = 0;
    if (++debugCounter % 100 == 0) // Print every 100 calls to avoid spam
    {
        printf("Speedometer: Raw=%.1f -> Filtered=%.1f km/h, Step=%d, Backlash=%s\n",
               originalSpeed, filteredSpeed, currentStep,
               inBacklashCompensation ? "ACTIVE" : "NONE");
    }

    // Clamp filtered speed to valid range
    filteredSpeed = std::clamp(filteredSpeed,
                               static_cast<double>(SpeedometerConfig::MIN_SPEED),
                               static_cast<double>(SpeedometerConfig::MAX_SPEED));

    // Calculate target step position using filtered speed
    int newStepToGo = convertToStep(filteredSpeed);

    // Determine the desired movement direction
    int desiredDirection = 0;
    if (newStepToGo > currentStep)
        desiredDirection = 1;
    else if (newStepToGo < currentStep)
        desiredDirection = -1;

    // Check if we need to start backlash compensation
    if (!inBacklashCompensation && desiredDirection != 0 &&
        desiredDirection != lastMovementDirection && lastMovementDirection != 0)
    {
        // Direction change detected - start backlash compensation
        inBacklashCompensation = true;
        backlashStepsRemaining = BACKLASH_STEPS;
        backlashDirection = desiredDirection;

        printf("Backlash compensation started: %d steps in direction %d\n",
               BACKLASH_STEPS, backlashDirection);
    }

    // Handle backlash compensation
    if (inBacklashCompensation)
    {
        if (backlashStepsRemaining > 0)
        {
            // Perform one backlash compensation step
            motor->setSpeed(SpeedometerConfig::MOTOR_SPEED_FINE); // Use fine speed for precision
            motor->step(backlashDirection);
            currentStep += backlashDirection;
            backlashStepsRemaining--;

            // Don't update stepToGo during backlash compensation
            return;
        }
        else
        {
            // Backlash compensation complete
            inBacklashCompensation = false;
            printf("Backlash compensation completed\n");
        }
    }

    // Normal movement logic (only when not in backlash compensation)
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
        lastMovementDirection = 1; // Track movement direction for backlash detection
    }
    else if (currentStep > newStepToGo)
    {
        motor->step(-1);
        currentStep--;
        lastMovementDirection = -1; // Track movement direction for backlash detection
    }
    // If currentStep == newStepToGo, no movement needed, don't change lastMovementDirection

    // Update stepToGo for next iteration
    stepToGo = newStepToGo;
}
