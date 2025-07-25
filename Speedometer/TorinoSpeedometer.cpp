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
#include "GPS.h"
#include "helpers/OdometerTypes.h"
#include "helpers/DisplayHelper.h"
#include "helpers/TripHelper.h"
#include "helpers/OdometerHelper.h"
#include "helpers/StorageHelper.h"
#include "helpers/TimeHelper.h"

int main()
{
    stdio_init_all();
    printf("=== Raspberry Pi Pico 2 Odometer Starting ===\n");

    // Initialize watchdog
    if (watchdog_enable_caused_reboot())
    {
        printf("Rebooted by watchdog!\n");
    }
    watchdog_enable(Config::WATCHDOG_TIMEOUT_MS, true);
    printf("Watchdog enabled with %d ms timeout\n", Config::WATCHDOG_TIMEOUT_MS);

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

    // Initialize trip values
    state.lastTrip1Km = state.trip1Km;
    state.lastTrip2Km = state.trip2Km;
    state.lastTrip3Km = state.trip3Km;
    state.lastSavedTrip1Km = state.trip1Km;
    state.lastSavedTrip2Km = state.trip2Km;
    state.lastSavedTrip3Km = state.trip3Km;

    Speedometer speedometer;
    GPS gps;

    // Initialize GPS
    if (!gps.initialize())
    {
        printf("ERROR: Failed to initialize GPS module!\n");
    }
    else
    {
        printf("GPS module initialized successfully\n");
        // Enable GPS debug mode to see raw NMEA sentences
        gps.setDebugMode(true);
    }

    // Initialize reset button
    Button resetButton(Config::Button::RESET_PARTIAL_PIN, true); // true for pull-up

    // Initialize I2C
    DisplayHelper::initializeI2C();

    SSD1306 lowerDisplay(i2c0, Config::I2C::I2C0_SDA, Config::I2C::I2C0_SCL);
    SSD1306 upperDisplay(i2c1, Config::I2C::I2C1_SDA, Config::I2C::I2C1_SCL);

    // Set up initial display values
    bool partialKmNeedsUpdate = false;
    bool totalKmNeedsUpdate = false;
    TripMode currentTripMode = TripMode::PARTIAL; // Currently selected trip odometer

    DisplayHelper::initializeDisplays(lowerDisplay, upperDisplay, state, currentTripMode);

    // HELPER: Set total kilometers to a fixed value
    // OdometerHelper::setTotalKilometers(state, storage, 1070.0);

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
        static bool longPressTriggered = false;

        if (resetButton.isPressed())
        {
            if (!wasButtonPressed)
            {
                // Button was just pressed, start timing
                buttonPressStartTime = currentTime;
                wasButtonPressed = true;
                longPressTriggered = false;
            }
            else if (currentTime - buttonPressStartTime >= Config::LONG_BUTTON_HOLD_TIME_MS && !longPressTriggered)
            {
                // Button has been held for 10 seconds - reserved for future use
                longPressTriggered = true;
            }
        }
        else
        {
            // Button was released - check what action to take based on hold duration
            if (wasButtonPressed && !longPressTriggered)
            {
                unsigned long holdDuration = currentTime - buttonPressStartTime;

                if (holdDuration >= Config::DISPLAY_INFO_HOLD_TIME_MS)
                {
                    // Button was held for 5+ seconds - reserved for future use
                }
                else if (holdDuration >= 3000)
                {
                    // Button was held for 3-5 seconds, reset the current trip odometer
                    TripHelper::resetTrip(state, currentTripMode);
                    partialKmNeedsUpdate = true;
                }
                else if (holdDuration < 1000)
                {
                    // Short press (< 1 second), cycle through trip modes
                    currentTripMode = static_cast<TripMode>((static_cast<int>(currentTripMode) + 1) % 6);
                    partialKmNeedsUpdate = true;
                }
            }

            // Reset button state
            wasButtonPressed = false;
            buttonPressStartTime = 0;
            longPressTriggered = false;
        }

        // Process GPS data and update gauge
        gps.update();
        const GPSData &gpsData = gps.getData();

        // Check for GPS fix state changes to trigger display updates
        static bool lastValidFix = false;
        if (gpsData.valid_fix != lastValidFix)
        {
            // GPS fix state changed - force display refresh
            partialKmNeedsUpdate = true;
            totalKmNeedsUpdate = true;
            lastValidFix = gpsData.valid_fix;
        }

        // Debug GPS status every 5 seconds
        static unsigned long lastGPSDebug = 0;
        if (currentTime - lastGPSDebug > 2000)
        {
            printf("GPS: Fix=%s, Sats=%d, Speed=%.1f km/h, Position=%.6f,%.6f, Time=%02d:%02d UTC\n",
                   gpsData.valid_fix ? "YES" : "NO",
                   gpsData.satellites_used,
                   gpsData.speed_kmh,
                   gpsData.latitude, gpsData.longitude,
                   gpsData.hour, gpsData.minute);
            lastGPSDebug = currentTime;
        }

        // Update speedometer gauge with GPS speed
        speedometer.loop(round(gpsData.speed_kmh));

        // Update state based on GPS data
        OdometerHelper::checkForDataChanges(state, gpsData, partialKmNeedsUpdate, totalKmNeedsUpdate, currentTripMode);

        // Update displays
        if (currentTime - lastDisplayUpdateTime > Config::DISPLAY_UPDATE_INTERVAL_MS)
        {
            char buffer[16];

            // Show GPS debug info only when acquiring signal (no valid fix)
            bool showGPSDebug = !gpsData.valid_fix;

            if (showGPSDebug)
            {
                // GPS acquiring signal - show progress messages
                if (gpsData.satellites_used == 0)
                {
                    upperDisplay.drawString(SSD1306_ALIGN_CENTER, "GPS", LiberationSansNarrow_Bold28);
                    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, "SEARCH", LiberationSansNarrow_Bold28);
                }
                else if (gpsData.satellites_used < 4)
                {
                    snprintf(buffer, sizeof(buffer), "SAT%d", gpsData.satellites_used);
                    upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, "WAIT", LiberationSansNarrow_Bold28);
                }
                else
                {
                    snprintf(buffer, sizeof(buffer), "SAT%d", gpsData.satellites_used);
                    upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, "CALC", LiberationSansNarrow_Bold28);
                }
            }
            else
            {
                // Normal odometer display mode - always update lower display
                if (currentTripMode == TripMode::SPEED)
                {
                    // Speed display mode - show current speed in lower display
                    snprintf(buffer, sizeof(buffer), "%d", (int)gpsData.speed_kmh);
                    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                }
                else if (currentTripMode == TripMode::TIME)
                {
                    // Time display mode - show current time in 24-hour format (HH:MM)
                    // Get time from GPS if available, otherwise use system time
                    if (gpsData.valid_fix && gpsData.hour != 255 && gpsData.minute != 255)
                    {
                        // Convert UTC time from GPS to local time
                        uint8_t local_hour, local_minute;
                        TimeHelper::convertUTCToLocal(gpsData.hour, gpsData.minute, local_hour, local_minute);
                        snprintf(buffer, sizeof(buffer), "%02d:%02d", local_hour, local_minute);
                    }
                    else
                    {
                        // Fallback to system time if GPS time not available
                        absolute_time_t now = get_absolute_time();
                        uint64_t us_since_boot = to_us_since_boot(now);
                        // Simple time calculation (this would need proper RTC for real time)
                        int seconds_since_boot = us_since_boot / 1000000;
                        int hours = (seconds_since_boot / 3600) % 24;
                        int minutes = (seconds_since_boot / 60) % 60;
                        snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
                    }
                    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                }
                else
                {
                    // Trip odometer modes (PARTIAL, TRIP1, TRIP2, TRIP3) - always update
                    TripHelper::getCurrentTripDisplayString(state, currentTripMode, buffer, sizeof(buffer));
                    lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                    partialKmNeedsUpdate = false;
                }

                // Always update total km display when we have valid fix
                snprintf(buffer, sizeof(buffer), "%d", (int)state.lastTotalKm);
                upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                totalKmNeedsUpdate = false;
            }
            lastDisplayUpdateTime = currentTime;
        }

        StorageHelper::saveDataIfNeeded(storage, state, currentTime, lastSaveTime);
    }

    return 0;
}