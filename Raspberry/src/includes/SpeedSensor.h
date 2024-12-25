#pragma once

#include "BCM2835Manager.h"
#include "Process.h"
class SpeedSensor : public Process
{
private:
    /* data */
    const RPiGPIOPin D0_PIN = RPI_GPIO_P1_16; // Pin for Digital Output (D0)

    uint32_t object_count = 0; // Counter for detected objects
    uint8_t last_state = LOW;  // Store the last state of the sensor
    uint64_t last_time = 0;    // Time of the last detection in microseconds

public:
    SpeedSensor(/* args */);
    ~SpeedSensor();
    void loop(EngineValues *) override;
};