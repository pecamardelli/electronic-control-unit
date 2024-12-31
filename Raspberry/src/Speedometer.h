#pragma once

#include "Process.h"
#include "Button.h"
#include "lib/Stepper/Stepper.h"
#include "lib/SSD1306/SSD1306.h"
struct Conversion
{
    float speed;
    int step;
};

class Speedometer : public Process
{
private:
    /* data */
    const RPiGPIOPin IN1 = RPI_V2_GPIO_P1_29; // GPIO 5
    const RPiGPIOPin IN2 = RPI_V2_GPIO_P1_31; // GPIO 6
    const RPiGPIOPin IN3 = RPI_V2_GPIO_P1_33; // GPIO 13
    const RPiGPIOPin IN4 = RPI_V2_GPIO_P1_37; // GPIO 26
    Stepper motor = Stepper(2038, IN1, IN3, IN2, IN4);
    Button button = Button(RPI_V2_GPIO_P1_07);
    int currentStep = 0;
    int stepOffset = 0;

    std::vector<Conversion>
        conversions = {
            {20, 85}, {30, 160}, {40, 233}, {50, 304}, {60, 380}, {70, 455}, {80, 526}, {90, 597}, {100, 672}, {110, 745}, {120, 825}, {130, 905}, {140, 980}, {150, 1056}, {160, 1129}, {170, 1204}, {180, 1280}, {190, 1353}, {200, 1430}, {210, 1505}, {220, 1590}, {230, 1673}, {240, 1800}};

    SSD1306 *ssd1306;

    void goToStartPosition();

public:
    Speedometer(/* args */);
    ~Speedometer();
    void loop(EngineValues *) override;
};