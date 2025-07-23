#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <bcm2835.h>

class DHT11
{
private:
    /* data */
    const int DHT_PIN = 0;

public:
    DHT11(/* args */);
    ~DHT11();
    void sendStartSignal();
    bool waitForState(int state, int timeout_us);
    bool readDHT11(uint8_t *data);
};
