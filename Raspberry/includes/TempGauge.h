#ifndef TEMP_GAUGE_H_
#define TEMP_GAUGE_H_

#include "Logger.h"
#include "../lib/Stepper/Stepper.h"
#include "Button.h"

class TempGauge
{
private:
    /* data */
    Logger logger = Logger("TempGauge");
    // Define GPIO pins for the ULN2003AN
    const uint8_t IN1 = RPI_V2_GPIO_P1_36; // GPIO 16
    const uint8_t IN2 = RPI_V2_GPIO_P1_38; // GPIO 20
    const uint8_t IN3 = RPI_V2_GPIO_P1_40; // GPIO 21
    const uint8_t IN4 = RPI_V2_GPIO_P1_37; // GPIO 26

    Stepper motor = Stepper(2038, IN1, IN3, IN2, IN4);
    Button button;
    uint32_t steps = 0;
    int direction = 1;

public:
    TempGauge(/* args */);
    ~TempGauge();
    void loop();
};

#endif