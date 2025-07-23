#ifndef TRIP_HELPER_H
#define TRIP_HELPER_H

#include <cstddef>
#include "OdometerTypes.h"

namespace TripHelper
{
    const char *getTripName(TripMode trip);
    double getCurrentTripValue(const OdometerState &state, TripMode trip);
    void getCurrentTripDisplayString(const OdometerState &state, TripMode trip, char *buffer, size_t bufferSize);
    void resetTrip(OdometerState &state, TripMode trip);
    void checkTripLimits(OdometerState &state, bool &partialKmNeedsUpdate);
}

#endif // TRIP_HELPER_H
