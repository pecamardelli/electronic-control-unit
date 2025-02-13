#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <bcm2835.h>

class DHT11
{
private:
    /* data */
    const RPiGPIOPin DHT_PIN = RPI_V2_GPIO_P1_32;

public:
    DHT11(/* args */);
    ~DHT11();
    void sendStartSignal();
    bool waitForState(int state, int timeout_us);
    bool readDHT11(uint8_t *data);
};
