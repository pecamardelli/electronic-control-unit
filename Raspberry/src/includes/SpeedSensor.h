#pragma once

#include <cmath>

#include "BCM2835Manager.h"
#include "Process.h"

class SpeedSensor : public Process
{
private:
    /* data */
    const RPiGPIOPin D0_PIN = RPI_GPIO_P1_16; // Pin for Digital Output (D0)
    // Constants for speed calculation
    double gearRatio = 43.0 / 13.0;       // Final gear ratio
    double tireWidth = 215.0;             // Tire width in mm
    double aspectRatio = 60.0;            // Tire aspect ratio as a percentage
    double rimDiameter = 15.0;            // Rim diameter in inches
    double transitionsPerLap = 4.0;       // Transitions per driveshaft lap
    double tireCircumference = 0.0;       // Tire circumference in mm
    double kilometersPerTransition = 0.0; // Distance covered per transition in km

    double calculateTireCircumference();
    double calculateSpeed(uint64_t);

public:
    SpeedSensor(/* args */);
    ~SpeedSensor();
    void loop(EngineValues *) override;
};