#include "DS3231.h"

DS3231::DS3231(uint8_t address) : i2cAddress(address), lastCompareTime(std::chrono::steady_clock::now())
{
    description = "DS3231";
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);
    compareInterval = config->get<int64_t>("compare_interval");

    compareTime();
}

DS3231::~DS3231()
{
}

void DS3231::setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
    bcm2835_i2c_setSlaveAddress(i2cAddress);

    char buffer[8] = {0};
    buffer[0] = 0x00;
    buffer[1] = decToBcd(second);
    buffer[2] = decToBcd(minute);
    buffer[3] = decToBcd(hour);
    buffer[4] = decToBcd(day);
    buffer[5] = decToBcd(date);
    buffer[6] = decToBcd(month);
    buffer[7] = decToBcd(year);

    if (bcm2835_i2c_write(buffer, 8) != BCM2835_I2C_REASON_OK)
    {
        logger->error("Could not set date and time!");
    }
    else
    {
        logger->info("Date and time successfully set!");
    }
}

std::string DS3231::getTime()
{
    char buffer[7] = {0};
    bcm2835_i2c_setSlaveAddress(i2cAddress);
    bcm2835_i2c_write_read_rs(&command, 1, buffer, 7);

    timeStruct.tm_sec = bcdToDec(buffer[0]);
    timeStruct.tm_min = bcdToDec(buffer[1]);
    timeStruct.tm_hour = bcdToDec(buffer[2]);
    timeStruct.tm_wday = bcdToDec(buffer[3]);
    timeStruct.tm_mday = bcdToDec(buffer[4]);
    timeStruct.tm_mon = bcdToDec(buffer[5]) - 1;
    timeStruct.tm_year = bcdToDec(buffer[6]) + 100;

    return tmToString(timeStruct);
}

uint8_t DS3231::bcdToDec(uint8_t val)
{
    return (val / 16 * 10) + (val % 16);
}

uint8_t DS3231::decToBcd(uint8_t val)
{
    return (val / 10 * 16) + (val % 10);
}

void DS3231::printTime()
{
    std::cout << getTime() << std::endl;
}

void DS3231::compareTime()
{
    // Compare only if the specified amount of seconds has passed.
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastCompareTime).count();

    if (elapsed < compareInterval)
        return;

    lastCompareTime = now; // Update last call time

    // Get current system time using std::chrono
    auto systemNow = std::chrono::system_clock::now();
    std::time_t systemTime = std::chrono::system_clock::to_time_t(systemNow);
    struct tm *systemTm = std::localtime(&systemTime);

    std::time_t rtcTime = std::mktime(&timeStruct);

    if (systemTime > rtcTime + 1) // System time is ahead by more than 1 second
    {
        logger->info("Updating DS3231 with system time.");
        setTime(systemTm->tm_hour, systemTm->tm_min, systemTm->tm_sec,
                systemTm->tm_wday, systemTm->tm_mday, systemTm->tm_mon + 1, systemTm->tm_year % 100);
    }
    else if (systemTime < rtcTime - 1) // System time is behind by more than 1 second
    {
        logger->info("Updating system time with DS3231 time.");
        setSystemTime(timeStruct);
    }
    else
    {
        std::cout << "System time and RTC time are synchronized within 1 second." << std::endl;
    }
}

bool DS3231::setSystemTime(const std::tm &newTime)
{
    struct timeval tv;
    tv.tv_sec = mktime(const_cast<std::tm *>(&newTime)); // Convert tm to time_t
    tv.tv_usec = 0;                                      // Microseconds set to 0

    if (settimeofday(&tv, nullptr) == 0)
    {
        logger->info("System time updated successfully!");
        return true;
    }
    else
    {
        logger->error("Failed to update system time!");
        return false;
    }
}

std::string DS3231::tmToString(const std::tm &timeStruct)
{
    std::ostringstream oss;
    oss << std::put_time(&timeStruct, format.c_str());
    return oss.str();
}