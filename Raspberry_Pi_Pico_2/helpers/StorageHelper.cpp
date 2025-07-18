#include "StorageHelper.h"
#include "../FlashStorage.h"
#include <stdio.h>

namespace StorageHelper
{
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
                // TODO: some logic or a message to display when an error occurred?
                printf("Failed to save odometer data\n");
            }
        }
    }
}
