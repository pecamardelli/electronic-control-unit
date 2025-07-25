#ifndef ODOMETER_TYPES_H
#define ODOMETER_TYPES_H

#include <cstdint>
#include "pico/stdlib.h"

// Configuration constants
namespace Config
{
    // Display thresholds and intervals
    constexpr double PARTIAL_UPDATE_THRESHOLD = 0.1; // Update display every 0.1 km
    constexpr double TOTAL_UPDATE_THRESHOLD = 1.0;   // Update display every 1.0 km
    constexpr int DISPLAY_UPDATE_INTERVAL_MS = 100;  // Update display every 100ms max
    constexpr int MIN_SAVE_INTERVAL_MS = 3000;       // Minimum time between saves
    constexpr double MIN_CHANGE_THRESHOLD = 0.01;    // Minimum change to trigger a save

    // Test mode configuration
    constexpr int TEST_MOTOR_SPEED = 4;             // Motor speed during test (4 RPM)
    constexpr int TEST_INTERVAL_SEC = 2;            // Delay between test positions (2 seconds)
    constexpr int LONG_BUTTON_HOLD_TIME_MS = 10000; // Time to hold button for long press (10 seconds)
    constexpr int DISPLAY_INFO_HOLD_TIME_MS = 5000; // Time to hold button to toggle info display (5 seconds)

    // Trip odometer limits
    constexpr double MAX_TRIP_VALUE = 1000.0; // Maximum value before auto-reset (applies to T1, T2, T3 only)

    // I2C configuration
    namespace I2C
    {
        constexpr int I2C0_SDA = 4;        // I2C0 SDA pin
        constexpr int I2C0_SCL = 5;        // I2C0 SCL pin
        constexpr int I2C1_SDA = 2;        // I2C1 SDA pin
        constexpr int I2C1_SCL = 3;        // I2C1 SCL pin
        constexpr uint BAUD_RATE = 400000; // I2C bus speed (400 kHz)
    }

    // Button configuration
    namespace Button
    {
        constexpr int RESET_PARTIAL_PIN = 17; // GPIO pin for reset button (with pull-up)
    }

    // Watchdog timeout in milliseconds
    constexpr uint32_t WATCHDOG_TIMEOUT_MS = 8000; // Watchdog timeout (8 seconds)

    // GPS accuracy and filtering configuration
    namespace GPS
    {
        constexpr uint8_t MIN_SATELLITES = 4;                // Minimum satellites for positioning
        constexpr double MAX_HDOP = 5.0;                     // Maximum HDOP for good accuracy
        constexpr double MIN_SPEED_KMPH = 2.0;               // Minimum speed for reliable distance calc
        constexpr double MAX_DISTANCE_JUMP_KM = 10.0;        // Maximum realistic distance per update
        constexpr double MIN_MOVEMENT_KM = 0.005;            // Minimum movement to filter GPS noise (5m)
        constexpr double SPEED_CONSISTENCY_THRESHOLD = 20.0; // Max difference between calculated and reported speed
    }

    // Timezone configuration
    namespace Time
    {
        constexpr int TIMEZONE_OFFSET_HOURS = -3;      // UTC offset in hours (Argentina: UTC-3)
        constexpr bool ENABLE_DAYLIGHT_SAVING = false; // Set to true if you want DST support
        constexpr int DST_OFFSET_HOURS = 1;            // Additional offset during DST
    }
}

// Enum to track which trip odometer is currently selected
enum class TripMode
{
    PARTIAL = 0, // Original partial odometer
    TRIP1 = 1,   // Trip 1
    TRIP2 = 2,   // Trip 2
    TRIP3 = 3,   // Trip 3
    SPEED = 4,   // Current speed display mode
    TIME = 5     // Current time display mode
};

// Structure to hold odometer state
struct OdometerState
{
    double totalKm;
    double currentTotalKm;
    double lastTotalKm;
    double partialKm;
    double currentPartialKm;
    double lastPartialKm;
    double lastSavedTotalKm;
    double lastSavedPartialKm;
    bool dataChanged;

    // Trip odometers
    double trip1Km;
    double trip2Km;
    double trip3Km;
    double currentTrip1Km;
    double currentTrip2Km;
    double currentTrip3Km;
    double lastTrip1Km;
    double lastTrip2Km;
    double lastTrip3Km;
    double lastSavedTrip1Km;
    double lastSavedTrip2Km;
    double lastSavedTrip3Km;

    OdometerState() : totalKm(0), currentTotalKm(0), lastTotalKm(0),
                      partialKm(0), currentPartialKm(0), lastPartialKm(0),
                      lastSavedTotalKm(0), lastSavedPartialKm(0),
                      dataChanged(false),
                      trip1Km(0), trip2Km(0), trip3Km(0),
                      currentTrip1Km(0), currentTrip2Km(0), currentTrip3Km(0),
                      lastTrip1Km(0), lastTrip2Km(0), lastTrip3Km(0),
                      lastSavedTrip1Km(0), lastSavedTrip2Km(0), lastSavedTrip3Km(0) {}
};

#endif // ODOMETER_TYPES_H
