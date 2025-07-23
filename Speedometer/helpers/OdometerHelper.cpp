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

        // Get distance traveled from GPS (in kilometers)
        static bool firstValidFix = true;
        static double lastLatitude = 0.0;
        static double lastLongitude = 0.0;

        double distanceCoveredKm = 0.0;

        if (firstValidFix)
        {
            // First GPS fix, just store position
            lastLatitude = gpsData.latitude;
            lastLongitude = gpsData.longitude;
            firstValidFix = false;
        }
        else
        {
            // Calculate distance from last position
            double distanceMeters = GPS::calculateDistance(lastLatitude, lastLongitude,
                                                           gpsData.latitude, gpsData.longitude);
            distanceCoveredKm = distanceMeters / 1000.0; // Convert to kilometers

            // Safety check: Reject unreasonable distances (likely GPS errors)
            if (distanceCoveredKm > 10.0) // More than 10 km in one GPS update is unrealistic
            {
                distanceCoveredKm = 0.0;
                // Don't update last position to avoid carrying forward the bad position
                return;
            }

            // Update last position
            lastLatitude = gpsData.latitude;
            lastLongitude = gpsData.longitude;

            // Only process if we've moved a reasonable distance (filter GPS noise)
            if (distanceCoveredKm < 0.001)
            { // Less than 1 meter
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
