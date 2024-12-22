#pragma once

#include "Logger.h"
#include "Process.h"
#include "System.h"

#include <iostream>
#include <fstream>
#include <string>
#include <bcm2835.h>
#include <unistd.h>

#define GPS_UART "/dev/serial0" // Raspberry Pi UART port
extern System *sys;
class GPS : public Process
{
private:
    /* data */
    Logger logger = Logger(description);
    std::string baudRate;

public:
    GPS(/* args */);
    ~GPS();

    void loop() override;
};