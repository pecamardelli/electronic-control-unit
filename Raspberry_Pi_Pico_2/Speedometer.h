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