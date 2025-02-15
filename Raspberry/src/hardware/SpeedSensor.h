#pragma once

#include <cmath>
#include <bcm2835.h>
#include "Process.h"
#include "helpers.h"

extern volatile SpeedSensorData *speedSensorData;

class SpeedSensor : public Process
{
private:
    /* data */
    const RPiGPIOPin D0_PIN = RPI_V2_GPIO_P1_13; // Pin for Digital Output (D0)
    // Constants for speed calculation
    double gearRatio = 43.0 / 13.0;           // Final gear ratio
    double tireWidth = 215.0;                 // Tire width in mm
    double aspectRatio = 60.0;                // Tire aspect ratio as a percentage
    double rimDiameter = 15.0;                // Rim diameter in inches
    double transitionsPerDriveshaftRev = 4.0; // Transitions per driveshaft lap
    double tireCircumference = 0.0;           // Tire circumference in mm
    double kilometersPerTransition = 0.0;     // Distance covered per transition in km
    double demultiplication = 2.8;            // Driveshaft revolutions per sensor revolutions.
    double driveshaftRevsPerSecond;           // Driveshaft revolutions per second
    double wheelRevsPerSecond;
    double metersPerSecond;
    double carStoppedInterval = 2;

    uint64_t lastTransitionDuration = 0;
    uint64_t elapsedTimeSinceLastTransition = 0;

    bool testEnabled = false;
    uint64_t testInterval = 0;
    double speedModifier = 0;
    double lowerSpeed = 0;
    double higherSpeed = 240;

    uint8_t lastState = LOW; // Store the last state of the sensor
    uint64_t lastTime = 0;   // Time of the last detection in microseconds
    uint8_t currentState;
    uint64_t currentTime;
    uint64_t elapsedTime;

    double calculateTireCircumference();
    double calculateSpeed(uint64_t);

public:
    SpeedSensor(/* args */);
    ~SpeedSensor();
    void loop() override;
};