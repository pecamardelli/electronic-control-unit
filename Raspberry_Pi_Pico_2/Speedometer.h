/**
 * @file Speedometer.h
 * @brief Header file for the Speedometer class
 *
 * This file defines the Speedometer class which controls a stepper motor-based
 * speed gauge. It inherits from the Gauge class and provides functionality for
 * converting speed values to motor steps and controlling the motor movement.
 */

#pragma once

#include <cstring>

#include "Gauge.h"

/**
 * @class Speedometer
 * @brief Controls a stepper motor-based speed gauge
 *
 * The Speedometer class manages a stepper motor that drives a mechanical speedometer
 * needle. It converts speed values to motor steps and dynamically adjusts the motor
 * speed based on the distance to the target position.
 */
class Speedometer : public Gauge
{
private:
    // Motor control pins
    const uint IN1 = 21;         ///< First motor control pin
    const uint IN2 = 20;         ///< Second motor control pin
    const uint IN3 = 19;         ///< Third motor control pin
    const uint IN4 = 18;         ///< Fourth motor control pin
    const uint INTERRUPTOR = 27; ///< Interrupt pin for position detection

    // State tracking
    uint64_t lastTotalMileage = 0; ///< Last recorded total mileage
    double speed = 0;              ///< Current speed value
    // double lastSpeed = 0;
    uint64_t lastStepTime = 0; ///< Timestamp of last motor step
    int stepToGo = 0;          ///< Target step position
    int currentStep = 0;       ///< Current step position

    // Speed filtering variables
    static constexpr int FILTER_BUFFER_SIZE = 5;  ///< Number of samples for moving average
    double speedBuffer[FILTER_BUFFER_SIZE] = {0}; ///< Circular buffer for speed samples
    int bufferIndex = 0;                          ///< Current index in the circular buffer
    bool bufferFilled = false;                    ///< Whether the buffer has been filled at least once
    double filteredSpeed = 0;                     ///< Current filtered speed value

private:
    /**
     * @brief Filters the raw GPS speed to reduce noise
     *
     * Applies multiple filtering techniques:
     * - Dead zone for speeds below threshold
     * - Moving average filter
     * - Hysteresis for start/stop detection
     *
     * @param rawSpeed The raw speed from GPS in km/h
     * @return The filtered speed in km/h
     */
    double filterSpeed(double rawSpeed);

public:
    /**
     * @brief Constructs a new Speedometer object
     *
     * Initializes the stepper motor and sets up the speed-to-step conversion table.
     */
    Speedometer();

    /**
     * @brief Destructor for the Speedometer object
     *
     * Ensures proper cleanup of resources.
     */
    ~Speedometer();

    /**
     * @brief Main control loop for the speedometer
     *
     * Updates the speedometer position based on the current speed.
     *
     * @param speed The current speed in kilometers per hour
     */
    void loop(double speed);
};