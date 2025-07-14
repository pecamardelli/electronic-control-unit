#include <stdio.h>
#include <iomanip>
#include <cmath>
#include <string>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "pico/time.h"

#include "Common.h"
#include "FlashStorage.h"
#include "Stepper.h"
#include "Speedometer.h"
#include "SpeedSensor.h"
#include "SSD1306.h"
#include "Button.h"

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
    constexpr int TEST_BUTTON_HOLD_TIME_MS = 10000; // Time to hold button for test mode (10 seconds)
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
}

// Enum to track which trip odometer is currently selected
enum class TripMode
{
    PARTIAL = 0, // Original partial odometer
    TRIP1 = 1,   // Trip 1
    TRIP2 = 2,   // Trip 2
    TRIP3 = 3    // Trip 3
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

// Function prototypes
void initializeI2C();
void initializeDisplays(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, const OdometerState &state, TripMode currentTrip);
void checkForDataChanges(OdometerState &state, const SpeedSensorData &sensorData, bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate, TripMode currentTrip);
void updateDisplaysIfNeeded(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, OdometerState &state,
                            bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate, unsigned long currentTime, unsigned long &lastDisplayUpdateTime);
void saveDataIfNeeded(FlashStorage &storage, OdometerState &state, unsigned long currentTime, unsigned long &lastSaveTime);
const char *getTripName(TripMode trip);
double getCurrentTripValue(const OdometerState &state, TripMode trip);
void getCurrentTripDisplayString(const OdometerState &state, TripMode trip, char *buffer, size_t bufferSize);
void resetTrip(OdometerState &state, TripMode trip);
void checkTripLimits(OdometerState &state, bool &partialKmNeedsUpdate);

int main()
{
    stdio_init_all();

    // Initialize watchdog
    if (watchdog_enable_caused_reboot())
    {
        printf("Rebooted by watchdog!\n");
    }
    watchdog_enable(Config::WATCHDOG_TIMEOUT_MS, true);

    OdometerState state;
    unsigned long lastSaveTime = 0;
    unsigned long lastDisplayUpdateTime = 0;
    bool testMode = false;
    unsigned long testModeStartTime = 0;

    FlashStorage storage(1024 * 1024, 16);

    // Try to read existing data
    if (!storage.readData(&state.totalKm, &state.partialKm))
    {
        state.totalKm = 0;
        state.partialKm = 0;
    }

    // Initialize with current values
    state.lastTotalKm = state.totalKm;
    state.lastPartialKm = state.partialKm;
    state.lastSavedTotalKm = state.totalKm;
    state.lastSavedPartialKm = state.partialKm;

    // Initialize trip values
    state.lastTrip1Km = state.trip1Km;
    state.lastTrip2Km = state.trip2Km;
    state.lastTrip3Km = state.trip3Km;
    state.lastSavedTrip1Km = state.trip1Km;
    state.lastSavedTrip2Km = state.trip2Km;
    state.lastSavedTrip3Km = state.trip3Km;

    state.partialKm = 761.2;
    state.trip1Km = 111.2;
    state.trip2Km = 22.3;
    state.trip3Km = 647.9;

    Speedometer speedometer;
    SpeedSensor speedSensor;
    SpeedSensorData speedSensorData = {0, 0.0, 0.0, 0.0};

    // Initialize reset button
    Button resetButton(Config::Button::RESET_PARTIAL_PIN, true); // true for pull-up

    // Initialize I2C
    initializeI2C();

    SSD1306 lowerDisplay(i2c0, Config::I2C::I2C0_SDA, Config::I2C::I2C0_SCL);
    SSD1306 upperDisplay(i2c1, Config::I2C::I2C1_SDA, Config::I2C::I2C1_SCL);

    // Set up initial display values
    bool partialKmNeedsUpdate = false;
    bool totalKmNeedsUpdate = false;
    bool infoDisplayEnabled = false;              // Flag to enable/disable info display mode
    TripMode currentTripMode = TripMode::PARTIAL; // Currently selected trip odometer

    initializeDisplays(lowerDisplay, upperDisplay, state, currentTripMode);

    // Main loop
    while (true)
    {
        // Feed the watchdog to prevent reset
        watchdog_update();

        unsigned long currentTime = to_ms_since_boot(get_absolute_time());

        // Check reset button
        resetButton.check();
        static unsigned long buttonPressStartTime = 0;
        static bool wasButtonPressed = false;
        static bool testModeTriggered = false;

        if (resetButton.isPressed())
        {
            if (!wasButtonPressed)
            {
                // Button was just pressed, start timing
                buttonPressStartTime = currentTime;
                wasButtonPressed = true;
                testModeTriggered = false;
            }
            else if (currentTime - buttonPressStartTime >= Config::TEST_BUTTON_HOLD_TIME_MS && !testModeTriggered)
            {
                // Button has been held for 10 seconds, enter test mode
                if (!testMode)
                {
                    testMode = true;
                    testModeStartTime = currentTime;
                    printf("Entering test mode...\n");
                    speedSensor.setTestMode(true, 0.0); // Start test mode with 0 km/h
                    testMode = false;
                    printf("Test mode completed.\n");
                    testModeTriggered = true;
                }
            }
        }
        else
        {
            // Button was released - check what action to take based on hold duration
            if (wasButtonPressed && !testModeTriggered)
            {
                unsigned long holdDuration = currentTime - buttonPressStartTime;

                if (holdDuration >= Config::DISPLAY_INFO_HOLD_TIME_MS)
                {
                    // Button was held for 5+ seconds, toggle info display mode
                    infoDisplayEnabled = !infoDisplayEnabled;

                    if (infoDisplayEnabled)
                    {
                        printf("Info display mode enabled\n");
                    }
                    else
                    {
                        printf("Info display mode disabled - returning to odometer\n");
                        // Force odometer display update when returning to normal mode
                        totalKmNeedsUpdate = true;
                        partialKmNeedsUpdate = true;
                    }
                }
                else if (holdDuration >= 3000)
                {
                    // Button was held for 3-5 seconds, reset the current trip odometer
                    resetTrip(state, currentTripMode);
                    partialKmNeedsUpdate = true;
                    printf("Trip %s reset\n", getTripName(currentTripMode));
                }
                else if (holdDuration < 1000)
                {
                    // Short press (< 1 second), cycle through trip modes
                    currentTripMode = static_cast<TripMode>((static_cast<int>(currentTripMode) + 1) % 4);
                    partialKmNeedsUpdate = true;
                    printf("Switched to %s\n", getTripName(currentTripMode));
                }
            }

            // Reset button state
            wasButtonPressed = false;
            buttonPressStartTime = 0;
            testModeTriggered = false;
        }

        // Process speed sensor and update gauge
        speedSensorData = speedSensor.loop();

        // Update speedometer gauge with current speed
        speedometer.loop(round(speedSensorData.speed));

        // Update state based on new sensor data
        checkForDataChanges(state, speedSensorData, partialKmNeedsUpdate, totalKmNeedsUpdate, currentTripMode);

        // Update displays
        if (currentTime - lastDisplayUpdateTime > Config::DISPLAY_UPDATE_INTERVAL_MS)
        {
            char buffer[16];

            if (infoDisplayEnabled)
            {
                // Info display mode: show real-time speed and transitions
                // Show current speed in upper display
                snprintf(buffer, sizeof(buffer), "%d", (int)speedSensorData.speed);
                upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);

                // Show total transitions in lower display
                snprintf(buffer, sizeof(buffer), "%llu", speedSensorData.transitions);
                lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
            }
            else
            {
                // Normal odometer display mode
                // In test mode, show speed in lower display
                if (speedSensor.isTestMode())
                {
                    snprintf(buffer, sizeof(buffer), "%d", (int)speedSensorData.speed);
                    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                }
                else if (partialKmNeedsUpdate)
                {
                    getCurrentTripDisplayString(state, currentTripMode, buffer, sizeof(buffer));
                    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                    partialKmNeedsUpdate = false;
                }

                if (totalKmNeedsUpdate)
                {
                    snprintf(buffer, sizeof(buffer), "%d", (int)state.lastTotalKm);
                    upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                    totalKmNeedsUpdate = false;
                }
            }

            lastDisplayUpdateTime = currentTime;
        }

        saveDataIfNeeded(storage, state, currentTime, lastSaveTime);
    }

    return 0;
}

void initializeI2C()
{
    // Initialize I2C0
    gpio_set_function(Config::I2C::I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(Config::I2C::I2C0_SCL, GPIO_FUNC_I2C);
    i2c_init(i2c0, Config::I2C::BAUD_RATE);

    // Initialize I2C1
    gpio_set_function(Config::I2C::I2C1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(Config::I2C::I2C1_SCL, GPIO_FUNC_I2C);
    i2c_init(i2c1, Config::I2C::BAUD_RATE);
}

void initializeDisplays(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, const OdometerState &state, TripMode currentTrip)
{
    char buffer[16]; // Buffer for formatted strings

    // Format and display current trip kilometers with identifier
    getCurrentTripDisplayString(state, currentTrip, buffer, sizeof(buffer));
    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);

    // Format and display total kilometers
    snprintf(buffer, sizeof(buffer), "%d", (int)state.totalKm);
    upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
}

void checkForDataChanges(OdometerState &state, const SpeedSensorData &sensorData, bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate, TripMode currentTrip)
{
    // Update current values for total and partial (partial always tracks)
    state.currentTotalKm = state.totalKm + sensorData.distanceCovered;
    state.currentPartialKm = state.partialKm + sensorData.distanceCovered;

    // Update trip odometers - only the selected trip accumulates distance
    if (currentTrip == TripMode::TRIP1)
    {
        state.currentTrip1Km = state.trip1Km + sensorData.distanceCovered;
    }
    else if (currentTrip == TripMode::TRIP2)
    {
        state.currentTrip2Km = state.trip2Km + sensorData.distanceCovered;
    }
    else if (currentTrip == TripMode::TRIP3)
    {
        state.currentTrip3Km = state.trip3Km + sensorData.distanceCovered;
    }

    // Check if data has changed enough to warrant saving
    if (fabs(state.currentTotalKm - state.lastSavedTotalKm) > Config::MIN_CHANGE_THRESHOLD ||
        fabs(state.currentPartialKm - state.lastSavedPartialKm) > Config::MIN_CHANGE_THRESHOLD ||
        fabs(state.currentTrip1Km - state.lastSavedTrip1Km) > Config::MIN_CHANGE_THRESHOLD ||
        fabs(state.currentTrip2Km - state.lastSavedTrip2Km) > Config::MIN_CHANGE_THRESHOLD ||
        fabs(state.currentTrip3Km - state.lastSavedTrip3Km) > Config::MIN_CHANGE_THRESHOLD)
    {
        state.dataChanged = true;
    }

    // Check if displays need updates based on current trip
    double currentTripValue = getCurrentTripValue(state, currentTrip);
    double lastTripValue = 0;

    switch (currentTrip)
    {
    case TripMode::PARTIAL:
        lastTripValue = state.lastPartialKm;
        break;
    case TripMode::TRIP1:
        lastTripValue = state.lastTrip1Km;
        break;
    case TripMode::TRIP2:
        lastTripValue = state.lastTrip2Km;
        break;
    case TripMode::TRIP3:
        lastTripValue = state.lastTrip3Km;
        break;
    }

    if (currentTripValue >= lastTripValue + Config::PARTIAL_UPDATE_THRESHOLD)
    {
        // Update the appropriate last value
        switch (currentTrip)
        {
        case TripMode::PARTIAL:
            state.lastPartialKm = state.currentPartialKm;
            break;
        case TripMode::TRIP1:
            state.lastTrip1Km = state.currentTrip1Km;
            break;
        case TripMode::TRIP2:
            state.lastTrip2Km = state.currentTrip2Km;
            break;
        case TripMode::TRIP3:
            state.lastTrip3Km = state.currentTrip3Km;
            break;
        }
        partialKmNeedsUpdate = true;
    }

    if (state.currentTotalKm >= state.lastTotalKm + Config::TOTAL_UPDATE_THRESHOLD)
    {
        state.lastTotalKm = floor(state.currentTotalKm);
        totalKmNeedsUpdate = true;
    }

    // Check if any trip has exceeded the maximum limit and auto-reset if needed
    checkTripLimits(state, partialKmNeedsUpdate);
}

void updateDisplaysIfNeeded(SSD1306 &lowerDisplay, SSD1306 &upperDisplay,
                            OdometerState &state, bool &partialKmNeedsUpdate,
                            bool &totalKmNeedsUpdate, unsigned long currentTime,
                            unsigned long &lastDisplayUpdateTime)
{
    // Only update displays periodically to avoid overwhelming the I2C bus
    if (currentTime - lastDisplayUpdateTime > Config::DISPLAY_UPDATE_INTERVAL_MS)
    {
        char buffer[16];

        if (partialKmNeedsUpdate)
        {
            snprintf(buffer, sizeof(buffer), "%.1f", state.lastPartialKm);
            lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
            partialKmNeedsUpdate = false;
        }

        if (totalKmNeedsUpdate)
        {
            snprintf(buffer, sizeof(buffer), "%d", (int)state.lastTotalKm);
            upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
            totalKmNeedsUpdate = false;
        }

        lastDisplayUpdateTime = currentTime;
    }
}

void saveDataIfNeeded(FlashStorage &storage, OdometerState &state,
                      unsigned long currentTime, unsigned long &lastSaveTime)
{
    // Save data to flash only when:
    // 1. Data has actually changed
    // 2. Enough time has passed since last save to prevent excessive writes
    if (state.dataChanged && (currentTime - lastSaveTime > Config::MIN_SAVE_INTERVAL_MS))
    {
        if (storage.saveData(state.currentTotalKm, state.currentPartialKm))
        {
            // Update saved values only on successful save
            state.lastSavedTotalKm = state.currentTotalKm;
            state.lastSavedPartialKm = state.currentPartialKm;
            state.lastSavedTrip1Km = state.currentTrip1Km;
            state.lastSavedTrip2Km = state.currentTrip2Km;
            state.lastSavedTrip3Km = state.currentTrip3Km;
            state.dataChanged = false;
            lastSaveTime = currentTime;
        }
        else
        {
            // TODO: some logic or a message to display when an error ocurred?
            printf("Failed to save odometer data\n");
        }
    }
}

const char *getTripName(TripMode trip)
{
    switch (trip)
    {
    case TripMode::PARTIAL:
        return "Partial";
    case TripMode::TRIP1:
        return "Trip1";
    case TripMode::TRIP2:
        return "Trip2";
    case TripMode::TRIP3:
        return "Trip3";
    default:
        return "Unknown";
    }
}

double getCurrentTripValue(const OdometerState &state, TripMode trip)
{
    switch (trip)
    {
    case TripMode::PARTIAL:
        return state.currentPartialKm;
    case TripMode::TRIP1:
        return state.currentTrip1Km;
    case TripMode::TRIP2:
        return state.currentTrip2Km;
    case TripMode::TRIP3:
        return state.currentTrip3Km;
    default:
        return 0.0;
    }
}

void getCurrentTripDisplayString(const OdometerState &state, TripMode trip, char *buffer, size_t bufferSize)
{
    double tripValue = getCurrentTripValue(state, trip);

    switch (trip)
    {
    case TripMode::PARTIAL:
        snprintf(buffer, bufferSize, "%.1f", tripValue);
        break;
    case TripMode::TRIP1:
        snprintf(buffer, bufferSize, "T1 %.1f", tripValue);
        break;
    case TripMode::TRIP2:
        snprintf(buffer, bufferSize, "T2 %.1f", tripValue);
        break;
    case TripMode::TRIP3:
        snprintf(buffer, bufferSize, "T3 %.1f", tripValue);
        break;
    default:
        snprintf(buffer, bufferSize, "?.? %.1f", tripValue);
        break;
    }
}

void resetTrip(OdometerState &state, TripMode trip)
{
    switch (trip)
    {
    case TripMode::PARTIAL:
        state.partialKm = 0;
        state.currentPartialKm = 0;
        state.lastPartialKm = 0;
        state.lastSavedPartialKm = 0;
        break;
    case TripMode::TRIP1:
        state.trip1Km = 0;
        state.currentTrip1Km = 0;
        state.lastTrip1Km = 0;
        state.lastSavedTrip1Km = 0;
        break;
    case TripMode::TRIP2:
        state.trip2Km = 0;
        state.currentTrip2Km = 0;
        state.lastTrip2Km = 0;
        state.lastSavedTrip2Km = 0;
        break;
    case TripMode::TRIP3:
        state.trip3Km = 0;
        state.currentTrip3Km = 0;
        state.lastTrip3Km = 0;
        state.lastSavedTrip3Km = 0;
        break;
    }
    state.dataChanged = true;
}

void checkTripLimits(OdometerState &state, bool &partialKmNeedsUpdate)
{
    // Note: Partial odometer is excluded from auto-reset since it has no prefix and can display more digits

    // Check Trip1
    if (state.currentTrip1Km >= Config::MAX_TRIP_VALUE)
    {
        resetTrip(state, TripMode::TRIP1);
        partialKmNeedsUpdate = true;
        printf("Trip1 auto-reset at %.1f km\n", Config::MAX_TRIP_VALUE);
    }

    // Check Trip2
    if (state.currentTrip2Km >= Config::MAX_TRIP_VALUE)
    {
        resetTrip(state, TripMode::TRIP2);
        partialKmNeedsUpdate = true;
        printf("Trip2 auto-reset at %.1f km\n", Config::MAX_TRIP_VALUE);
    }

    // Check Trip3
    if (state.currentTrip3Km >= Config::MAX_TRIP_VALUE)
    {
        resetTrip(state, TripMode::TRIP3);
        partialKmNeedsUpdate = true;
        printf("Trip3 auto-reset at %.1f km\n", Config::MAX_TRIP_VALUE);
    }
}