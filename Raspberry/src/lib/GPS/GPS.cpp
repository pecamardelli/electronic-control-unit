#include "GPS.h"

GPS::GPS(/* args */)
{
    description = "GPS";
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);

    loopInterval = config->get<useconds_t>("loop_interval");
    baudRate = config->get<std::string>("baud_rate");
    // Configure the Raspberry Pi UART interface (if needed)
    std::string command = "stty -F /dev/serial0 " + baudRate + " raw -echo";
    system(command.c_str());
}

GPS::~GPS()
{
}

void GPS::loop(EngineValues *engineValues)
{
    std::ifstream gpsStream(GPS_UART, std::ios::in);
    std::string line;

    if (!gpsStream.is_open())
    {
        logger->error("Error: Could not open GPS UART port.");
    }

    if (std::getline(gpsStream, line))
    {
        // Filter NMEA sentences if needed, e.g., $GPGGA or $GPRMC
        if (line.find("$GPGGA") != std::string::npos || line.find("$GPRMC") != std::string::npos)
        {
            std::cout << "GPS Data: " << line << std::endl;
        }
    }

    usleep(loopInterval);
}