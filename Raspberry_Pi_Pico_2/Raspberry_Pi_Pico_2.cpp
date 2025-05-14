#include <stdio.h>
#include <iomanip>
#include <cmath>
#include "pico/stdlib.h"

#include "Common.h"
#include "FlashStorage.h"
#include "Stepper.h"
#include "Speedometer.h"
#include "SpeedSensor.h"
#include "SSD1306.h"

#include "Common.h"

const int I2C0_SDA = 4;
const int I2C0_SCL = 5;
const int I2C1_SDA = 2;
const int I2C1_SCL = 3;
const uint I2C_BAUD_RATE = 400000; // 400 kHz

int main()
{
    stdio_init_all();

    double totalKm = 0;
    double currentTotalKm = 0;
    int lastTotalKm = NULL;
    double partialKm = 0;
    double currentPartialKm = 0;
    double lastPartialKm = NULL;
    bool saveOdo = false;

    unsigned long lastCheck = 0;

    FlashStorage storage(1024 * 1024, 16);

    // Try to read existing data
    if (!storage.readData(&totalKm, &partialKm))
    {
        totalKm = 0;
        partialKm = 0;
    }

    Speedometer speedometer;
    SpeedSensor speedSensor;
    SpeedSensorData speedSensorData = {0, 0.0, 0.0, 0.0};

    // Initialize I2C pins
    gpio_set_function(I2C0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C0_SCL, GPIO_FUNC_I2C);
    i2c_init(i2c0, I2C_BAUD_RATE);

    gpio_set_function(I2C1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL, GPIO_FUNC_I2C);
    i2c_init(i2c1, I2C_BAUD_RATE);

    SSD1306 lowerDisplay(i2c0, I2C0_SDA, I2C0_SCL);
    SSD1306 upperDisplay(i2c1, I2C1_SDA, I2C1_SCL);

    while (true)
    {
        speedSensorData = speedSensor.loop();
        currentTotalKm = totalKm + speedSensorData.distanceCovered;
        currentPartialKm = partialKm + speedSensorData.distanceCovered;

        if (lastPartialKm == NULL || currentPartialKm >= lastPartialKm + 0.1)
        {
            lastPartialKm = currentPartialKm;
            lowerDisplay.drawString(SSD1306_ALIGN_CENTER, std::to_string(lastPartialKm).c_str(), LiberationSansNarrow_Bold28);
            lowerDisplay.display();
            saveOdo = true;
        }

        if (lastTotalKm == NULL || currentTotalKm >= lastTotalKm + 1)
        {
            lastTotalKm = round(currentTotalKm);
            upperDisplay.drawString(SSD1306_ALIGN_CENTER, std::to_string(lastTotalKm).c_str(), LiberationSansNarrow_Bold28);
            upperDisplay.display();
            saveOdo = true;
        }

        speedometer.loop(round(speedSensorData.speed));

        if (saveOdo)
        {
            storage.saveData(currentTotalKm, currentPartialKm);
            saveOdo = false;
        }
    }

    return 0;
}