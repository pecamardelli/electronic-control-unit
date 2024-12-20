#pragma once

#include "Logger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <bcm2835.h>

#define GPS_UART "/dev/serial0" // Raspberry Pi UART port

class GPS
{
private:
    /* data */
    const std::string description = "GPS";
    Logger logger = Logger(description);

public:
    GPS(/* args */);
    ~GPS();
    void readData();
};