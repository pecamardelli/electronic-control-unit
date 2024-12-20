#include "GPS.h"

GPS::GPS(/* args */)
{
    description = "GPS";
    loopInterval = sys.getConfigValue<useconds_t>(description, "loop_interval");
    baudRate = sys.getConfigValue<std::string>(description, "baud_rate");
    // Configure the Raspberry Pi UART interface (if needed)
    std::string command = "stty - F /dev/serial0 " + baudRate + " raw - echo";
    system(command.c_str());
}

GPS::~GPS()
{
}

void GPS::loop()
{
    std::ifstream gpsStream(GPS_UART, std::ios::in);
    std::string line;

    if (!gpsStream.is_open())
    {
        logger.error("Error: Could not open GPS UART port.");
    }

    if (std::getline(gpsStream, line))
    {
        // Filter NMEA sentences if needed, e.g., $GPGGA or $GPRMC
        if (line.find("$GPGGA") != std::string::npos || line.find("$GPRMC") != std::string::npos)
        {
        }
        std::cout << "GPS Data: " << line << std::endl;
    }

    usleep(loopInterval);
}