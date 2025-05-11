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

    double totalKm = 1;
    int lastTotalKm = 0;
    double partialKm = 0;
    double lastPartialKm = partialKm;
    double lastSpeed = 0;
    double roundedSpeed;
    double speedModifier = 0.1;

    unsigned long lastCheck = 0;

    FlashStorage storage(1024 * 1024, 16);

    // Try to read existing data
    if (!storage.readData(&totalKm, &partialKm))
    {
        totalKm = 999;
        partialKm = 0;
    }
    // storage.saveData(totalKm, partialKm);

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

        if (lastPartialKm != partialKm)
        {
            lowerDisplay.drawString(SSD1306_ALIGN_CENTER, std::to_string(round(totalKm)).c_str(), LiberationSansNarrow_Bold28);
            lowerDisplay.display();
            lastPartialKm = partialKm;
        }

        if (lastTotalKm != totalKm)
        {

            upperDisplay.drawString(SSD1306_ALIGN_CENTER, std::to_string(lastTotalKm).c_str(), LiberationSansNarrow_Bold28);
            upperDisplay.display();
            lastTotalKm = round(totalKm);
        }

        speedometer.loop(round(speedSensorData.speed));

        // if (lastCheck + 10000 < time_us_64())
        // {
        //     lastCheck = time_us_64();
        //     // partialKm = partialKm + 1; // Simulate speed increase
        //     // totalKm = totalKm + 1;     // Simulate total km increase
        //     speed += speedModifier; // Simulate speed increase
        //     if (speed >= 240)
        //     {
        //         speedModifier = -0.1;
        //     }
        //     else if (speed <= 0)
        //     {
        //         speedModifier = 0.1;
        //     }
        // }
    }

    return 0;
}