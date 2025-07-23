#ifndef TIME_HELPER_H
#define TIME_HELPER_H

#include <cstdint>

namespace TimeHelper
{
    /**
     * @brief Convert UTC time to local time based on configured timezone offset
     *
     * @param utc_hour UTC hour (0-23)
     * @param utc_minute UTC minute (0-59)
     * @param local_hour Reference to store local hour result
     * @param local_minute Reference to store local minute result
     */
    void convertUTCToLocal(uint8_t utc_hour, uint8_t utc_minute, uint8_t &local_hour, uint8_t &local_minute);
}

#endif // TIME_HELPER_H
