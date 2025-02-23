#pragma once

#include <bcm2835.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#include "Base.h"

class DS3231 : Base
{
public:
    DS3231(uint8_t address = 0x68);
    ~DS3231();

    void setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t date, uint8_t month, uint8_t year);
    std::string getTime();
    void printTime();
    void compareTime();
    bool updateTimeFromNTP();
    std::string tmToString(const std::tm &);

private:
    uint8_t i2cAddress;
    struct tm timeStruct;
    const char *ntpServers[4] = {"1.ar.pool.ntp.org", "2.south-america.pool.ntp.org", "1.south-america.pool.ntp.org", "pool.ntp.org"};
    char command = 0x00;
    time_t serverTimeout = 3;
    const std::string &format = "%d/%m/%Y %H:%M:%S";
    std::chrono::steady_clock::time_point lastCompareTime;
    int64_t compareInterval = 60;

    uint8_t bcdToDec(uint8_t val);
    uint8_t decToBcd(uint8_t val);
    bool setSystemTime(const std::tm &);
};
