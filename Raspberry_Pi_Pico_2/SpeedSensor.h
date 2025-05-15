#pragma once

#include <cmath>
#include <numbers>
#include "pico/stdlib.h"
#include "Common.h"

// Configuration namespace for speed sensor settings
namespace SpeedSensorConfig
{
    // Pin configuration
    constexpr uint DEFAULT_SENSOR_PIN = 28; // Default pin for Digital Output (D0)

    // Default tire specifications
    constexpr double DEFAULT_TIRE_WIDTH = 215.0;  // Tire width in mm
    constexpr double DEFAULT_ASPECT_RATIO = 60.0; // Tire aspect ratio as a percentage
    constexpr double DEFAULT_RIM_DIAMETER = 15.0; // Rim diameter in inches

    // Gear and sensor specifications
    constexpr double DEFAULT_GEAR_RATIO = 43.0 / 13.0;     // Final gear ratio
    constexpr double DEFAULT_DEMULTIPLICATION = 2.8;       // Driveshaft revolutions per sensor revolutions
    constexpr double TRANSITIONS_PER_DRIVESHAFT_REV = 4.0; // Transitions per driveshaft lap

    // Speed limits and thresholds
    constexpr double MIN_SPEED = 0.0;            // Minimum valid speed in km/h
    constexpr double MAX_SPEED = 240.0;          // Maximum valid speed in km/h
    constexpr double CAR_STOPPED_INTERVAL = 2.0; // Seconds to consider car stopped

    // Debounce settings
    constexpr uint64_t DEBOUNCE_TIME_US = 1000; // Debounce time in microseconds
}

/**
 * @brief Structure to hold tire specifications
 */
struct TireSpecs
{
    double width;       // Tire width in mm
    double aspectRatio; // Aspect ratio as a percentage
    double rimDiameter; // Rim diameter in inches

    TireSpecs(double w = SpeedSensorConfig::DEFAULT_TIRE_WIDTH,
              double ar = SpeedSensorConfig::DEFAULT_ASPECT_RATIO,
              double rd = SpeedSensorConfig::DEFAULT_RIM_DIAMETER)
        : width(w), aspectRatio(ar), rimDiameter(rd) {}
};

/**
 * @brief Class to handle speed and distance measurements from a hall effect sensor
 */
class SpeedSensor
{
private:
    // Pin configuration
    const uint sensorPin;

    // Tire and gear specifications
    const TireSpecs tireSpecs;
    const double gearRatio;
    const double demultiplication;
    const double transitionsPerDriveshaftRev;

    // Calculated values
    double tireCircumference;       // Tire circumference in mm
    double kilometersPerTransition; // Distance covered per transition in km

    // Speed calculation variables
    double driveshaftRevsPerSecond;
    double wheelRevsPerSecond;
    double metersPerSecond;

    // Timing variables
    uint64_t lastTransitionDuration;
    uint64_t elapsedTimeSinceLastTransition;
    uint64_t lastDebounceTime;

    // Sensor state
    bool currentState;
    bool lastState;
    bool debouncedState;
    uint64_t lastTime;
    uint64_t currentTime;

    // Test mode variables
    bool testEnabled;
    double speedModifier;
    double testSpeed;

    // Sensor data
    SpeedSensorData speedSensorData;

    // Private methods
    double calculateTireCircumference() const;
    double calculateSpeed(uint64_t elapsedTime) const;
    bool isSpeedValid(double speed) const;
    void updateTestMode();
    void updateRealMode();
    void debounceInput();

public:
    /**
     * @brief Construct a new Speed Sensor object
     *
     * @param pin GPIO pin number for the sensor
     * @param specs Tire specifications
     * @param gearRatio Final gear ratio
     * @param demultiplication Driveshaft revolutions per sensor revolutions
     */
    SpeedSensor(uint pin = SpeedSensorConfig::DEFAULT_SENSOR_PIN,
                const TireSpecs &specs = TireSpecs(),
                double gearRatio = SpeedSensorConfig::DEFAULT_GEAR_RATIO,
                double demultiplication = SpeedSensorConfig::DEFAULT_DEMULTIPLICATION);

    ~SpeedSensor();

    /**
     * @brief Main loop function to update speed and distance measurements
     * @return SpeedSensorData Current speed and distance data
     */
    SpeedSensorData loop();

    /**
     * @brief Enable or disable test mode
     * @param enable Whether to enable test mode
     * @param initialSpeed Initial speed for test mode (km/h)
     */
    void setTestMode(bool enable, double initialSpeed = 0.0);

    /**
     * @brief Check if test mode is enabled
     * @return bool True if test mode is enabled, false otherwise
     */
    bool isTestMode() const { return testEnabled; }

    /**
     * @brief Reset the sensor data
     */
    void reset();
};