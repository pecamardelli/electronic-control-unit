#ifndef ODOMETER_HELPER_H
#define ODOMETER_HELPER_H

#include "../GPS.h"
#include "OdometerTypes.h"

// Forward declarations
class FlashStorage;

namespace OdometerHelper
{
    void checkForDataChanges(OdometerState &state, const GPSData &gpsData, bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate, TripMode currentTrip);
    void setTotalKilometers(OdometerState &state, FlashStorage &storage, double newTotalKm);
}

#endif // ODOMETER_HELPER_H
