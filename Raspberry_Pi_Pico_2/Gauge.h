/**
 * @file Gauge.h
 * @brief Header file for the Gauge base class
 *
 * This file defines the Gauge class which serves as a base class for different types
 * of mechanical gauges controlled by stepper motors. It provides common functionality
 * for motor control, position tracking, and calibration.
 */

#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <sys/unistd.h>
#include <iostream>

#include "Stepper.h"
#include "Button.h"

/**
 * @struct Conversion
 * @brief Structure for mapping values to motor steps
 *
 * This structure defines a mapping between a value (e.g., speed, temperature)
 * and the corresponding number of motor steps needed to reach that position.
 */
struct Conversion
{
    float value; ///< The value to be displayed (e.g., speed in km/h)
    int step;    ///< The corresponding motor step position
};

/**
 * @class Gauge
 * @brief Base class for mechanical gauges controlled by stepper motors
 *
 * The Gauge class provides common functionality for controlling mechanical gauges
 * using stepper motors. It handles motor initialization, position tracking,
 * value-to-step conversion, and calibration.
 */
class Gauge
{
private:
    /* data */

public:
    /**
     * @brief Constructs a new Gauge object
     */
    Gauge();

    /**
     * @brief Destructor for the Gauge object
     */
    ~Gauge();

    /**
     * @brief Enters calibration mode for the gauge
     *
     * Allows manual adjustment of the gauge position for calibration purposes.
     * In calibration mode, the user can:
     * - Move the needle to specific positions
     * - Return to the start position
     * - Exit calibration mode
     */
    void calibrate();

protected:
    std::unique_ptr<Stepper> motor;      ///< Stepper motor controller
    std::unique_ptr<Button> interruptor; ///< Button for position detection
    std::vector<Conversion> conversions; ///< Value-to-step conversion table
    int currentStep = 0;                 ///< Current motor step position
    int stepOffset = 0;                  ///< Offset for zero position

    /**
     * @brief Initializes the gauge
     *
     * Sets up the motor and finds the initial position using the interruptor.
     */
    void init();

    /**
     * @brief Moves the gauge to its start position
     *
     * Uses the interruptor to find the zero position and applies any
     * configured offset.
     */
    void goToStartPosition();

    /**
     * @brief Converts a value to the corresponding motor step
     *
     * Uses the conversion table to map a value to the appropriate
     * motor step position.
     *
     * @param value The value to convert (e.g., speed, temperature)
     * @return int The corresponding motor step position
     */
    int convertToStep(double value);

    /**
     * @brief Tests the gauge movement
     *
     * Moves the gauge through all positions in the conversion table
     * to verify proper operation.
     *
     * @param motorSpeed The speed to use for the test
     * @param testInterval The delay between positions in seconds
     */
    void test(long motorSpeed, unsigned int testInterval);
};