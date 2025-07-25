#include "OdometerHelper.h"
#include "../FlashStorage.h"
#include "TripHelper.h"
#include <cmath>
#include <stdio.h>

namespace OdometerHelper
{
    void checkForDataChanges(OdometerState &state, const GPSData &gpsData, bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate, TripMode currentTrip)
    {
        // Only update distances if GPS has a valid fix
        if (!gpsData.valid_fix)
            return;

        // GPS Accuracy Filtering - Check if GPS data is reliable enough for odometer use

        // 1. Minimum satellite requirement for accurate positioning
        if (gpsData.satellites_used < Config::GPS::MIN_SATELLITES)
        {
            printf("GPS: Insufficient satellites (%d) for accurate positioning\n", gpsData.satellites_used);
            return;
        }

        // 2. HDOP (Horizontal Dilution of Precision) filtering
        // HDOP < 2.0 = Excellent, < 5.0 = Good, < 10.0 = Moderate, > 10.0 = Poor
        if (gpsData.hdop > Config::GPS::MAX_HDOP)
        {
            printf("GPS: Poor accuracy (HDOP=%.1f), skipping distance calculation\n", gpsData.hdop);
            return;
        }

        // 3. Speed-based reliability check - very low speeds have higher GPS error
        if (gpsData.speed_kmh < Config::GPS::MIN_SPEED_KMPH)
        {
            printf("GPS: Speed too low (%.1f km/h) for reliable distance calculation\n", gpsData.speed_kmh);
            return;
        }

        // Get distance traveled from GPS (in kilometers)
        static bool firstValidFix = true;
        static double lastLatitude = 0.0;
        static double lastLongitude = 0.0;
        static uint32_t lastUpdateTime = 0;

        double distanceCoveredKm = 0.0;
        uint32_t currentTime = to_ms_since_boot(get_absolute_time());

        if (firstValidFix)
        {
            // First GPS fix, just store position
            lastLatitude = gpsData.latitude;
            lastLongitude = gpsData.longitude;
            lastUpdateTime = currentTime;
            firstValidFix = false;
        }
        else
        {
            // Calculate time difference for speed validation
            uint32_t timeDiffMs = currentTime - lastUpdateTime;
            double timeDiffHours = timeDiffMs / (1000.0 * 3600.0);

            // Calculate distance from last position
            double distanceMeters = GPS::calculateDistance(lastLatitude, lastLongitude,
                                                           gpsData.latitude, gpsData.longitude);
            distanceCoveredKm = distanceMeters / 1000.0; // Convert to kilometers

            // Enhanced safety checks for GPS errors

            // 4. Maximum distance per update (based on realistic vehicle speeds)
            if (distanceCoveredKm > Config::GPS::MAX_DISTANCE_JUMP_KM)
            {
                printf("GPS: Unrealistic distance jump (%.3f km), likely GPS error\n", distanceCoveredKm);
                distanceCoveredKm = 0.0;
                return; // Don't update last position to avoid carrying forward the bad position
            }

            // 5. Speed consistency check - compare calculated speed vs reported GPS speed
            if (timeDiffHours > 0)
            {
                double calculatedSpeedKmh = distanceCoveredKm / timeDiffHours;
                double speedDifference = fabs(calculatedSpeedKmh - gpsData.speed_kmh);

                // Allow some tolerance, but flag major discrepancies
                if (speedDifference > Config::GPS::SPEED_CONSISTENCY_THRESHOLD && calculatedSpeedKmh > 5.0)
                {
                    printf("GPS: Speed inconsistency - Calculated: %.1f km/h, Reported: %.1f km/h\n",
                           calculatedSpeedKmh, gpsData.speed_kmh);
                    // Still process but with reduced confidence
                }
            }

            // Update last position and time
            lastLatitude = gpsData.latitude;
            lastLongitude = gpsData.longitude;
            lastUpdateTime = currentTime;

            // 6. Minimum movement threshold to filter GPS noise/drift
            if (distanceCoveredKm < Config::GPS::MIN_MOVEMENT_KM)
            {
                distanceCoveredKm = 0.0;
            }
        }

        // Update current values for total and partial (partial always tracks)
        state.currentTotalKm = state.totalKm + distanceCoveredKm;
        state.currentPartialKm = state.partialKm + distanceCoveredKm;

        // Update trip odometers - only the selected trip accumulates distance
        if (currentTrip == TripMode::TRIP1)
        {
            state.currentTrip1Km = state.trip1Km + distanceCoveredKm;
        }
        else if (currentTrip == TripMode::TRIP2)
        {
            state.currentTrip2Km = state.trip2Km + distanceCoveredKm;
        }
        else if (currentTrip == TripMode::TRIP3)
        {
            state.currentTrip3Km = state.trip3Km + distanceCoveredKm;
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
        double currentTripValue = TripHelper::getCurrentTripValue(state, currentTrip);
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
        case TripMode::SPEED:
        case TripMode::TIME:
            // These modes don't have trip values to compare
            lastTripValue = 0;
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
            case TripMode::SPEED:
            case TripMode::TIME:
                // These modes don't update trip values
                break;
            }
            partialKmNeedsUpdate = true;
        }

        if (state.currentTotalKm >= state.lastTotalKm + Config::TOTAL_UPDATE_THRESHOLD)
        {
            state.lastTotalKm = floor(state.currentTotalKm);
            totalKmNeedsUpdate = true;
        }

        // Update the actual stored values if distance was covered
        if (distanceCoveredKm > 0)
        {
            state.totalKm = state.currentTotalKm;
            state.partialKm = state.currentPartialKm;

            switch (currentTrip)
            {
            case TripMode::TRIP1:
                state.trip1Km = state.currentTrip1Km;
                break;
            case TripMode::TRIP2:
                state.trip2Km = state.currentTrip2Km;
                break;
            case TripMode::TRIP3:
                state.trip3Km = state.currentTrip3Km;
                break;
            case TripMode::PARTIAL:
            case TripMode::SPEED:
            case TripMode::TIME:
                // These modes don't have separate trip storage
                break;
            }
        }

        // Check if any trip has exceeded the maximum limit and auto-reset if needed
        TripHelper::checkTripLimits(state, partialKmNeedsUpdate);
    }

    void setTotalKilometers(OdometerState &state, FlashStorage &storage, double newTotalKm)
    {
        printf("Setting total kilometers from %.1f to %.1f\n", state.totalKm, newTotalKm);

        // Update all total-related values
        state.totalKm = newTotalKm;
        state.currentTotalKm = newTotalKm;
        state.lastTotalKm = newTotalKm;
        state.lastSavedTotalKm = newTotalKm;

        // Mark data as changed to trigger save
        state.dataChanged = true;

        // Immediately save to flash storage
        if (storage.saveData(state.totalKm, state.currentPartialKm))
        {
            printf("Total kilometers successfully set to %.1f and saved to flash\n", newTotalKm);
            state.dataChanged = false;
            state.lastSavedTotalKm = state.totalKm;
        }
        else
        {
            printf("ERROR: Failed to save new total kilometers to flash!\n");
        }
    }
}
