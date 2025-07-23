#include "TripHelper.h"
#include <stdio.h>

namespace TripHelper
{
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
        case TripMode::SPEED:
            return "Speed";
        case TripMode::TIME:
            return "Time";
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
        case TripMode::SPEED:
            // Speed mode doesn't have anything to reset
            return;
        case TripMode::TIME:
            // Time mode doesn't have anything to reset
            return;
        }
        state.dataChanged = true;
    }

    void checkTripLimits(OdometerState &state, bool &partialKmNeedsUpdate)
    {
        // Note: Partial odometer is excluded from auto-reset since it has no prefix and can display more digits

        // Check Trip1
        if (state.trip1Km >= Config::MAX_TRIP_VALUE)
        {
            state.trip1Km = 0;
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
}
