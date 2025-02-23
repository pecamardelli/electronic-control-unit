#include "DS3231.h"

DS3231::DS3231(uint8_t address) : i2cAddress(address), lastCompareTime(std::chrono::steady_clock::now())
{
    description = "DS3231";
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);
    serverTimeout = config->get<time_t>("ntp_server_timeout");
    compareInterval = config->get<int64_t>("compare_interval");

    compareTime();
}

DS3231::~DS3231()
{
}

void DS3231::setTime(uint8_t hour, uint8_t minute, uint8_t second, uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
    logger->info("Setting the DS3231 date and time...");

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

bool DS3231::updateTimeFromNTP()
{
    const int NTP_PORT = 123;
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd < 0)
        return false;

    struct timeval timeout;
    timeout.tv_sec = serverTimeout;
    timeout.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    struct sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(NTP_PORT);

    uint8_t ntpPacket[48] = {0};
    ntpPacket[0] = 0b11100011; // LI, Version, Mode

    for (const char *server : ntpServers)
    {
        std::cout << "Trying NTP server: " << server << std::endl;
        inet_pton(AF_INET, server, &serverAddr.sin_addr);
        sendto(sockfd, ntpPacket, sizeof(ntpPacket), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        struct sockaddr_in responseAddr;
        socklen_t addrLen = sizeof(responseAddr);
        if (recvfrom(sockfd, ntpPacket, sizeof(ntpPacket), 0, (struct sockaddr *)&responseAddr, &addrLen) > 0)
        {
            close(sockfd);
            uint32_t timestamp = ntohl(*(uint32_t *)&ntpPacket[40]);
            timestamp -= 2208988800U; // Convert NTP to Unix time

            // Convert NTP timestamp to chrono time_point (system_clock)
            std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(timestamp);

            // Adjust for GMT-3 (subtract 3 hours from the time_point)
            tp -= std::chrono::hours(3);

            // Convert time_point to a time_t for printing
            std::time_t adjustedTime = std::chrono::system_clock::to_time_t(tp);

            // Print the adjusted time
            std::cout << "NTP Time Received (GMT-3): "
                      << std::put_time(std::localtime(&adjustedTime), "%d/%m/%Y %H:%M:%S") << std::endl;

            // Set the time (using the adjusted time, if needed)
            struct tm *ntpTime = std::localtime(&adjustedTime);
            setTime(ntpTime->tm_hour, ntpTime->tm_min, ntpTime->tm_sec,
                    ntpTime->tm_wday, ntpTime->tm_mday, ntpTime->tm_mon + 1, ntpTime->tm_year % 100);
            return true;
        }
        else
        {
            std::cout << "Failed to receive response from: " << server << std::endl;
        }
    }

    close(sockfd);
    return false;
}

void DS3231::compareTime()
{
    // Compare only if the specified amount of seconds has passed.
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastCompareTime).count();
    std::cout << "Elapsed: " << elapsed << " - CompareInterval: " << compareInterval << std::endl;

    if (elapsed < compareInterval)
        return;

    lastCompareTime = now; // Update last call time

    // Get current system time using std::chrono
    auto systemNow = std::chrono::system_clock::now();
    std::time_t systemTime = std::chrono::system_clock::to_time_t(systemNow);
    struct tm *systemTm = std::localtime(&systemTime);

    // Convert system time to tm structure
    std::cout << "System Date: " << tmToString(*systemTm) << std::endl;
    std::cout << "DS3231 Date: " << getTime() << std::endl;

    // Compare the system time and RTC time
    if (std::mktime(systemTm) > std::mktime(&timeStruct))
    {
        logger->info("Updating DS3231 with system time.");
        setTime(systemTm->tm_hour, systemTm->tm_min, systemTm->tm_sec,
                systemTm->tm_wday, systemTm->tm_mday, systemTm->tm_mon + 1, systemTm->tm_year % 100);
    }
    else if (std::mktime(systemTm) < std::mktime(&timeStruct))
    {
        logger->info("Updating system time with DS3231 time.");
        setSystemTime(timeStruct);
        // std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(std::mktime(&rtcTm));
        // std::this_thread::sleep_until(tp); // Wait until the RTC time is reached
    }
    else
    {
        std::cout << "System time and RTC time are the same." << std::endl;
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