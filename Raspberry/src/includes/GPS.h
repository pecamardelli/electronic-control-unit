#ifndef GPS_H_
#define GPS_H_

#include "Common.h"
#include "Logger.h"

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

#endif