#ifndef STORAGE_HELPER_H
#define STORAGE_HELPER_H

#include "OdometerTypes.h"

// Forward declarations
class FlashStorage;

namespace StorageHelper
{
    void saveDataIfNeeded(FlashStorage &storage, OdometerState &state, unsigned long currentTime, unsigned long &lastSaveTime);
}

#endif // STORAGE_HELPER_H
