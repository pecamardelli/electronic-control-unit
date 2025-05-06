#include <stdio.h>
#include "pico/stdlib.h"
#include "Stepper.h"
#include "Speedometer.h"
#include "SSD1306.h"

const int I2C0_SDA = 16;
const int I2C0_SCL = 17;
const int I2C1_SDA = 2;
const int I2C1_SCL = 3;

int main()
{
    // Initialize standard I/O over USB
    stdio_init_all();
    sleep_ms(2000); // Give time for initialization
    int totalKm = 860;
    float partialKm = 145.3;

    // Initialize I2C pins
    i2c_init(i2c0, 100000); // 400 kHz
    i2c_init(i2c1, 100000); // 400 kHz

    // Speedometer speedometer;
    SSD1306 upperDisplay(i2c0, I2C0_SDA, I2C0_SCL);
    SSD1306 lowerDisplay(i2c1, I2C1_SDA, I2C1_SCL);

    while (true)
    {
        // Draw some text
        lowerDisplay.drawString(SSD1306_ALIGN_CENTER, std::to_string(totalKm).c_str(), LiberationSansNarrow_Bold28);
        lowerDisplay.display();

        // Draw some text
        upperDisplay.drawString(SSD1306_ALIGN_CENTER, std::to_string(partialKm).c_str(), LiberationSansNarrow_Bold28);

        // // Update the display
        upperDisplay.display();

        // speedometer.loop();
        partialKm += 0.1; // Simulate speed increase
        totalKm += 1;     // Simulate total km increase
        sleep_ms(100);
    }

    return 0;
}