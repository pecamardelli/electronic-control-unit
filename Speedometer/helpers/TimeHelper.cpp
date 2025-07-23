#include "TimeHelper.h"
#include "OdometerTypes.h"

namespace TimeHelper
{
    void convertUTCToLocal(uint8_t utc_hour, uint8_t utc_minute, uint8_t &local_hour, uint8_t &local_minute)
    {
        int total_minutes = utc_hour * 60 + utc_minute;
        total_minutes += Config::Time::TIMEZONE_OFFSET_HOURS * 60;

        // TODO: Add DST support if needed
        // if (Config::Time::ENABLE_DAYLIGHT_SAVING && isDST())
        //     total_minutes += Config::Time::DST_OFFSET_HOURS * 60;

        // Handle day overflow/underflow
        if (total_minutes < 0)
            total_minutes += 24 * 60; // Add 24 hours
        else if (total_minutes >= 24 * 60)
            total_minutes -= 24 * 60; // Subtract 24 hours

        local_hour = total_minutes / 60;
        local_minute = total_minutes % 60;
    }
}
