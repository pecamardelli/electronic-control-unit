#include "GPS.h"

GPS::GPS(/* args */)
{
    // Configure the Raspberry Pi UART interface (if needed)
    system("stty -F " GPS_UART " 9600 raw -echo"); // Set baud rate to 9600
}

GPS::~GPS()
{
}

void GPS::readData()
{
    std::ifstream gpsStream(GPS_UART, std::ios::in);
    if (!gpsStream.is_open())
    {
        logger.error("Error: Could not open GPS UART port.");
        return;
    }

    std::string line;
    while (true)
    {
        if (std::getline(gpsStream, line))
        {
            // Filter NMEA sentences if needed, e.g., $GPGGA or $GPRMC
            if (line.find("$GPGGA") != std::string::npos || line.find("$GPRMC") != std::string::npos)
            {
                std::cout << "GPS Data: " << line << std::endl;
            }
        }
    }

    gpsStream.close();
}