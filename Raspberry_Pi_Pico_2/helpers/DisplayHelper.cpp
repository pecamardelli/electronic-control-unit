#include "DisplayHelper.h"
#include "TripHelper.h"
#include "hardware/i2c.h"

namespace DisplayHelper
{
    void initializeI2C()
    {
        // Initialize I2C0
        gpio_set_function(Config::I2C::I2C0_SDA, GPIO_FUNC_I2C);
        gpio_set_function(Config::I2C::I2C0_SCL, GPIO_FUNC_I2C);
        i2c_init(i2c0, Config::I2C::BAUD_RATE);

        // Initialize I2C1
        gpio_set_function(Config::I2C::I2C1_SDA, GPIO_FUNC_I2C);
        gpio_set_function(Config::I2C::I2C1_SCL, GPIO_FUNC_I2C);
        i2c_init(i2c1, Config::I2C::BAUD_RATE);
    }

    void initializeDisplays(SSD1306 &lowerDisplay, SSD1306 &upperDisplay, const OdometerState &state, TripMode currentTrip)
    {
        char buffer[16]; // Buffer for formatted strings

        // Format and display current trip kilometers with identifier
        TripHelper::getCurrentTripDisplayString(state, currentTrip, buffer, sizeof(buffer));
        lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);

        // Format and display total kilometers
        snprintf(buffer, sizeof(buffer), "%d", (int)state.totalKm);
        upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
    }

    void updateDisplaysIfNeeded(SSD1306 &lowerDisplay, SSD1306 &upperDisplay,
                                OdometerState &state, bool &partialKmNeedsUpdate,
                                bool &totalKmNeedsUpdate, unsigned long currentTime,
                                unsigned long &lastDisplayUpdateTime)
    {
        // Only update displays periodically to avoid overwhelming the I2C bus
        if (currentTime - lastDisplayUpdateTime > Config::DISPLAY_UPDATE_INTERVAL_MS)
        {
            char buffer[16];

            if (partialKmNeedsUpdate)
            {
                snprintf(buffer, sizeof(buffer), "%.1f", state.lastPartialKm);
                lowerDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                partialKmNeedsUpdate = false;
            }

            if (totalKmNeedsUpdate)
            {
                snprintf(buffer, sizeof(buffer), "%d", (int)state.lastTotalKm);
                upperDisplay.drawString(SSD1306_ALIGN_CENTER, buffer, LiberationSansNarrow_Bold28);
                totalKmNeedsUpdate = false;
            }

            lastDisplayUpdateTime = currentTime;
        }
    }
}
