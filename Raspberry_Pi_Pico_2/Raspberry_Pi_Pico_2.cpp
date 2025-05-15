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

    // I2C configuration
    namespace I2C
    {
        constexpr int I2C0_SDA = 4;
        constexpr int I2C0_SCL = 5;
        constexpr int I2C1_SDA = 2;
        constexpr int I2C1_SCL = 3;
        constexpr uint BAUD_RATE = 400000; // 400 kHz
    }

    // Button configuration
    namespace Button
    {
        constexpr int RESET_PARTIAL_PIN = 6; // GPIO pin for reset button
    }

    // Watchdog timeout in milliseconds
    constexpr uint32_t WATCHDOG_TIMEOUT_MS = 8000;
}

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

    OdometerState() : totalKm(0), currentTotalKm(0), lastTotalKm(0),
                      partialKm(0), currentPartialKm(0), lastPartialKm(0),
                      lastSavedTotalKm(0), lastSavedPartialKm(0),
                      dataChanged(false) {}
};

// Function prototypes
void initializeI2C();
void initializeDisplays(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, const OdometerState &state);
void checkForDataChanges(OdometerState &state, const SpeedSensorData &sensorData, bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate);
void updateDisplaysIfNeeded(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, OdometerState &state,
                            bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate, unsigned long currentTime, unsigned long &lastDisplayUpdateTime);
void saveDataIfNeeded(FlashStorage &storage, OdometerState &state, unsigned long currentTime, unsigned long &lastSaveTime);

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
    initializeDisplays(lowerDisplay, upperDisplay, state);

    bool partialKmNeedsUpdate = false;
    bool totalKmNeedsUpdate = false;

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

        if (resetButton.isPressed())
        {
            if (!wasButtonPressed)
            {
                // Button was just pressed, start timing
                buttonPressStartTime = currentTime;
                wasButtonPressed = true;
            }
            else if (currentTime - buttonPressStartTime >= 3000)
            {
                // Button has been held for 3 seconds, reset the partial odometer
                state.partialKm = 0;
                state.currentPartialKm = 0;
                state.lastPartialKm = 0;
                state.lastSavedPartialKm = 0;
                state.dataChanged = true;
                partialKmNeedsUpdate = true;

                // Reset button state
                wasButtonPressed = false;
                buttonPressStartTime = 0;
            }
        }
        else
        {
            // Button is not pressed, reset state
            wasButtonPressed = false;
            buttonPressStartTime = 0;
        }

        // Poll the speed sensor without delays to avoid missing pulses
        speedSensorData = speedSensor.loop();

        // Update state based on new sensor data
        checkForDataChanges(state, speedSensorData, partialKmNeedsUpdate, totalKmNeedsUpdate);

        // Update speedometer gauge
        speedometer.loop(round(speedSensorData.speed));

        updateDisplaysIfNeeded(lowerDisplay, upperDisplay, state,
                               partialKmNeedsUpdate, totalKmNeedsUpdate,
                               currentTime, lastDisplayUpdateTime);

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

void initializeDisplays(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, const OdometerState &state)
{
    char buffer[16]; // Buffer for formatted strings

    // Format and display partial kilometers
    snprintf(buffer, sizeof(buffer), "%.1f", state.partialKm);
    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);

    // Format and display total kilometers
    snprintf(buffer, sizeof(buffer), "%d", (int)state.totalKm);
    upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
}

void checkForDataChanges(OdometerState &state, const SpeedSensorData &sensorData, bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate)
{
    // Update current values
    state.currentTotalKm = state.totalKm + sensorData.distanceCovered;
    state.currentPartialKm = state.partialKm + sensorData.distanceCovered;

    // Check if data has changed enough to warrant saving
    if (fabs(state.currentTotalKm - state.lastSavedTotalKm) > Config::MIN_CHANGE_THRESHOLD ||
        fabs(state.currentPartialKm - state.lastSavedPartialKm) > Config::MIN_CHANGE_THRESHOLD)
    {
        state.dataChanged = true;
    }

    // Check if displays need updates
    if (state.currentPartialKm >= state.lastPartialKm + Config::PARTIAL_UPDATE_THRESHOLD)
    {
        state.lastPartialKm = state.currentPartialKm;
        partialKmNeedsUpdate = true;
    }

    if (state.currentTotalKm >= state.lastTotalKm + Config::TOTAL_UPDATE_THRESHOLD)
    {
        state.lastTotalKm = floor(state.currentTotalKm);
        totalKmNeedsUpdate = true;
    }
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