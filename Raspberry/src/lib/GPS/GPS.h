#pragma once

#include "Logger.h"
#include "Process.h"
#include "../../common.h"

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#define GPS_UART "/dev/serial0" // Raspberry Pi UART port

class GPS : public Process
{
private:
    /* data */
    std::string baudRate;

public:
    GPS(/* args */);
    ~GPS();

    void loop(EngineValues *) override;
};