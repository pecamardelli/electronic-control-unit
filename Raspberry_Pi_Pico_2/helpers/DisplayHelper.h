#ifndef DISPLAY_HELPER_H
#define DISPLAY_HELPER_H

#include "../SSD1306.h"
#include "../Common.h"
#include "../GPS.h"
#include "OdometerTypes.h"

namespace DisplayHelper
{
    void initializeI2C();
    void initializeDisplays(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, const OdometerState &state, TripMode currentTrip);
    void updateDisplaysIfNeeded(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, OdometerState &state,
                                bool &partialKmNeedsUpdate, bool &totalKmNeedsUpdate, unsigned long currentTime, unsigned long &lastDisplayUpdateTime);
}

#endif // DISPLAY_HELPER_H
