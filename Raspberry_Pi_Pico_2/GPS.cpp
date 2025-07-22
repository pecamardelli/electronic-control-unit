#include "GPS.h"
#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <math.h>

GPS::GPS() : uart(uart0), buffer_index(0), prev_latitude(0.0), prev_longitude(0.0), prev_position_valid(false), debug_mode(false)
{
    memset(rx_buffer, 0, sizeof(rx_buffer));
    memset(last_raw_sentence, 0, sizeof(last_raw_sentence));
}

GPS::~GPS()
{
    // Cleanup if needed
}

bool GPS::initialize()
{
    // Initialize UART for GPS communication
    uart_init(uart, GPSConfig::BAUD_RATE);

    // Set up GPIO pins for UART
    gpio_set_function(GPSConfig::TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(GPSConfig::RX_PIN, GPIO_FUNC_UART);

    // Configure UART parameters: 8 data bits, 1 stop bit, no parity
    uart_set_hw_flow(uart, false, false);
    uart_set_format(uart, 8, 1, UART_PARITY_NONE);

    // Enable UART FIFO
    uart_set_fifo_enabled(uart, true);

    printf("GPS: Initialized on UART%d (TX: GPIO%d, RX: GPIO%d) at %d baud\n",
           GPSConfig::UART_ID, GPSConfig::TX_PIN, GPSConfig::RX_PIN, GPSConfig::BAUD_RATE);

    // Wait a moment for GPS to stabilize
    sleep_ms(1000);

    // Optionally configure GPS settings
    setUpdateRate(1); // 1 Hz update rate

    return true;
}

void GPS::update()
{
    // Read available data from UART
    while (uart_is_readable(uart))
    {
        char c = uart_getc(uart);

        // Handle NMEA sentence building
        if (c == '$')
        {
            // Start of new NMEA sentence
            buffer_index = 0;
            rx_buffer[buffer_index++] = c;
        }
        else if (c == '\r' || c == '\n')
        {
            // End of NMEA sentence
            if (buffer_index > 0)
            {
                rx_buffer[buffer_index] = '\0';

                // Parse the complete NMEA sentence
                if (parseNMEA(rx_buffer))
                {
                    current_data.data_updated = true;
                }

                buffer_index = 0;
            }
        }
        else if (buffer_index < GPSConfig::BUFFER_SIZE - 1)
        {
            // Add character to buffer
            rx_buffer[buffer_index++] = c;
        }
        else
        {
            // Buffer overflow, reset
            buffer_index = 0;
        }
    }
}

bool GPS::parseNMEA(const char *sentence)
{
    if (!sentence || strlen(sentence) < 6)
        return false;

    // Store the raw sentence for debugging
    strncpy(last_raw_sentence, sentence, sizeof(last_raw_sentence) - 1);
    last_raw_sentence[sizeof(last_raw_sentence) - 1] = '\0';

    // Print raw sentence if debug mode is enabled
    // if (debug_mode)
    // {
    //     printf("RAW GPS: %s\n", sentence);
    // }

    // Validate checksum
    if (!validateChecksum(sentence))
        return false;

    // Determine sentence type and parse accordingly
    if (strncmp(sentence, "$GPGGA", 6) == 0 || strncmp(sentence, "$GNGGA", 6) == 0)
    {
        return parseGGA(sentence);
    }
    else if (strncmp(sentence, "$GPRMC", 6) == 0 || strncmp(sentence, "$GNRMC", 6) == 0)
    {
        return parseRMC(sentence);
    }
    else if (strncmp(sentence, "$GPGSA", 6) == 0 || strncmp(sentence, "$GNGSA", 6) == 0)
    {
        return parseGSA(sentence);
    }

    return false;
}

bool GPS::parseGGA(const char *sentence)
{
    // $GPGGA,hhmmss.ss,ddmm.mmmm,N,dddmm.mmmm,E,q,ss,h.h,a.a,M,g.g,M,d.d,ssss*hh
    char field[32];

    // Field 1: Time
    if (extractField(sentence, 1, field, sizeof(field)) && strlen(field) >= 6)
    {
        current_data.hour = parseUint8(field);
        current_data.minute = parseUint8(field + 2);
        current_data.second = parseUint8(field + 4);
    }

    // Field 2: Latitude
    if (extractField(sentence, 2, field, sizeof(field)) && strlen(field) > 0)
    {
        double lat = parseDouble(field);
        current_data.latitude = convertDMStoDD(lat);

        // Field 3: Latitude direction
        if (extractField(sentence, 3, field, sizeof(field)) && field[0] == 'S')
        {
            current_data.latitude = -current_data.latitude;
        }
    }

    // Field 4: Longitude
    if (extractField(sentence, 4, field, sizeof(field)) && strlen(field) > 0)
    {
        double lon = parseDouble(field);
        current_data.longitude = convertDMStoDD(lon);

        // Field 5: Longitude direction
        if (extractField(sentence, 5, field, sizeof(field)) && field[0] == 'W')
        {
            current_data.longitude = -current_data.longitude;
        }
    }

    // Field 6: Fix quality
    if (extractField(sentence, 6, field, sizeof(field)))
    {
        current_data.fix_quality = parseUint8(field);
        current_data.valid_fix = (current_data.fix_quality > 0);

        // Debug: Print fix quality parsing
        if (debug_mode)
        {
            printf("GPS: Fix quality field='%s', parsed=%d, valid_fix=%s\n",
                   field, current_data.fix_quality, current_data.valid_fix ? "true" : "false");
        }
    }

    // Field 7: Number of satellites
    if (extractField(sentence, 7, field, sizeof(field)))
    {
        current_data.satellites_used = parseUint8(field);
    }

    // Field 8: HDOP
    if (extractField(sentence, 8, field, sizeof(field)))
    {
        current_data.hdop = parseDouble(field);
    }

    // Field 9: Altitude
    if (extractField(sentence, 9, field, sizeof(field)))
    {
        current_data.altitude = parseDouble(field);
    }

    return true;
}

bool GPS::parseRMC(const char *sentence)
{
    // $GPRMC,hhmmss.ss,A,ddmm.mmmm,N,dddmm.mmmm,E,s.s,c.c,ddmmyy,d.d,E,m*hh
    char field[32];

    // Field 2: Status (A = active, V = void)
    if (extractField(sentence, 2, field, sizeof(field)))
    {
        bool status_valid = (field[0] == 'A');
        if (!status_valid)
            return false;
    }

    // Field 7: Speed in knots
    if (extractField(sentence, 7, field, sizeof(field)))
    {
        double speed_knots = parseDouble(field);
        current_data.speed_kmh = speed_knots * 1.852; // Convert knots to km/h
    }

    // Field 8: Course
    if (extractField(sentence, 8, field, sizeof(field)))
    {
        current_data.course = parseDouble(field);
    }

    // Field 9: Date
    if (extractField(sentence, 9, field, sizeof(field)) && strlen(field) >= 6)
    {
        current_data.day = parseUint8(field);
        current_data.month = parseUint8(field + 2);
        current_data.year = 2000 + parseUint8(field + 4);
    }

    return true;
}

bool GPS::parseGSA(const char *sentence)
{
    // $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39
    // This sentence provides DOP values and active satellites
    // For now, we'll just extract basic info since GGA provides most of what we need
    return true;
}

double GPS::convertDMStoDD(double dms)
{
    // Convert degrees-minutes-seconds format to decimal degrees
    // Input format: DDMM.MMMM or DDDMM.MMMM
    int degrees = (int)(dms / 100);
    double minutes = dms - (degrees * 100);
    return degrees + (minutes / 60.0);
}

uint8_t GPS::parseUint8(const char *str)
{
    if (!str || strlen(str) < 1)
        return 0;

    // Handle single digit
    if (strlen(str) == 1)
        return (uint8_t)(str[0] - '0');

    // Handle two digits
    return (uint8_t)((str[0] - '0') * 10 + (str[1] - '0'));
}

uint16_t GPS::parseUint16(const char *str)
{
    return (uint16_t)atoi(str);
}

double GPS::parseDouble(const char *str)
{
    return atof(str);
}

bool GPS::extractField(const char *sentence, int field_num, char *output, size_t output_size)
{
    if (!sentence || !output || output_size == 0)
        return false;

    const char *ptr = sentence;
    int current_field = 0;

    // Skip to the desired field
    while (*ptr && current_field < field_num)
    {
        if (*ptr == ',')
            current_field++;
        ptr++;
    }

    if (current_field != field_num)
        return false;

    // Extract field data
    size_t i = 0;
    while (*ptr && *ptr != ',' && *ptr != '*' && i < output_size - 1)
    {
        output[i++] = *ptr++;
    }
    output[i] = '\0';

    return true;
}

uint8_t GPS::calculateChecksum(const char *sentence)
{
    uint8_t checksum = 0;

    // Start after '$' and stop before '*'
    for (int i = 1; sentence[i] && sentence[i] != '*'; i++)
    {
        checksum ^= sentence[i];
    }

    return checksum;
}

bool GPS::validateChecksum(const char *sentence)
{
    const char *checksum_ptr = strchr(sentence, '*');
    if (!checksum_ptr)
        return false;

    uint8_t calculated = calculateChecksum(sentence);
    uint8_t received = (uint8_t)strtol(checksum_ptr + 1, NULL, 16);

    return calculated == received;
}

std::string GPS::getPositionString() const
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.6f, %.6f", current_data.latitude, current_data.longitude);
    return std::string(buffer);
}

std::string GPS::getTimeString() const
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d",
             current_data.hour, current_data.minute, current_data.second);
    return std::string(buffer);
}

std::string GPS::getDateString() const
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d",
             current_data.day, current_data.month, current_data.year);
    return std::string(buffer);
}

double GPS::getDistanceFromPrevious()
{
    if (!prev_position_valid || !current_data.valid_fix)
        return 0.0;

    double distance = calculateDistance(prev_latitude, prev_longitude,
                                        current_data.latitude, current_data.longitude);

    // Update previous position
    prev_latitude = current_data.latitude;
    prev_longitude = current_data.longitude;

    return distance;
}

double GPS::calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    // Haversine formula for calculating distance between two GPS coordinates
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
                   sin(dLon / 2) * sin(dLon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return GPSConfig::EARTH_RADIUS_KM * c * 1000; // Return distance in meters
}

std::string GPS::getSignalQuality() const
{
    if (!current_data.valid_fix)
        return "No Fix";

    if (current_data.satellites_used >= 8)
        return "Excellent";
    else if (current_data.satellites_used >= 6)
        return "Good";
    else if (current_data.satellites_used >= 4)
        return "Fair";
    else
        return "Poor";
}

bool GPS::sendCommand(const char *command)
{
    if (!command)
        return false;

    uart_puts(uart, command);
    uart_puts(uart, "\r\n");

    return true;
}

bool GPS::setUpdateRate(uint8_t rate_hz)
{
    if (rate_hz < 1 || rate_hz > 10)
        return false;

    // Send PMTK command to set update rate
    char command[32];
    int interval_ms = 1000 / rate_hz;
    snprintf(command, sizeof(command), "$PMTK220,%d*", interval_ms);

    // Calculate and append checksum
    uint8_t checksum = calculateChecksum(command);
    snprintf(command + strlen(command), 4, "%02X", checksum);

    return sendCommand(command);
}

bool GPS::enableNMEASentence(const char *sentence_type, bool enable)
{
    // This would send PMTK commands to enable/disable specific NMEA sentences
    // Implementation depends on specific GPS module capabilities
    return true;
}
