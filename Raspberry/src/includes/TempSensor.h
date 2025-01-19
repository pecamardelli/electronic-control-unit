#pragma once

#include <iostream>
#include <chrono>
#include <thread>

#include "BCM2835Manager.h"

class TempSensor
{
private:
    /* data */
    const RPiGPIOPin DHT_PIN = RPI_V2_GPIO_P1_32;

public:
    TempSensor(/* args */);
    ~TempSensor();
    void sendStartSignal();
    bool waitForState(int state, int timeout_us);
    bool readDHT11(uint8_t *data);
};
