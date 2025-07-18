#ifndef GPS_H
#define GPS_H

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include <string>
#include <cmath>

// GPS configuration constants
namespace GPSConfig
{
    constexpr uint UART_ID = 0;                // UART0 for GPS
    constexpr uint BAUD_RATE = 9600;           // Standard GPS baud rate
    constexpr uint TX_PIN = 0;                 // GPIO 0 for TX (Pico TX -> GPS RX)
    constexpr uint RX_PIN = 1;                 // GPIO 1 for RX (Pico RX <- GPS TX)
    constexpr uint BUFFER_SIZE = 256;          // NMEA sentence buffer size
    constexpr uint MAX_SATELLITES = 12;        // Maximum satellites to track
    constexpr double EARTH_RADIUS_KM = 6371.0; // Earth radius in kilometers
}

// GPS data structure
struct GPSData
{
    // Position data
    double latitude;  // Latitude in degrees
    double longitude; // Longitude in degrees
    double altitude;  // Altitude in meters

    // Speed and course
    double speed_kmh; // Speed in km/h
    double course;    // Course over ground in degrees

    // Time data
    uint8_t hour;   // UTC hour (0-23)
    uint8_t minute; // UTC minute (0-59)
    uint8_t second; // UTC second (0-59)
    uint8_t day;    // Day of month (1-31)
    uint8_t month;  // Month (1-12)
    uint16_t year;  // Year (e.g., 2025)

    // Quality indicators
    uint8_t satellites_used; // Number of satellites used in fix
    uint8_t fix_quality;     // GPS fix quality (0=invalid, 1=GPS, 2=DGPS)
    double hdop;             // Horizontal dilution of precision

    // Status flags
    bool valid_fix;    // True if GPS has valid fix
    bool data_updated; // True if new data received

    // Constructor
    GPSData() : latitude(0.0), longitude(0.0), altitude(0.0),
                speed_kmh(0.0), course(0.0),
                hour(0), minute(0), second(0), day(0), month(0), year(0),
                satellites_used(0), fix_quality(0), hdop(99.9),
                valid_fix(false), data_updated(false) {}
};

class GPS
{
private:
    uart_inst_t *uart;
    char rx_buffer[GPSConfig::BUFFER_SIZE];
    uint buffer_index;
    GPSData current_data;

    // Previous position for distance calculation
    double prev_latitude;
    double prev_longitude;
    bool prev_position_valid;

    // Private methods for NMEA parsing
    bool parseNMEA(const char *sentence);
    bool parseGGA(const char *sentence); // Global positioning system fix data
    bool parseRMC(const char *sentence); // Recommended minimum specific GPS data
    bool parseGSA(const char *sentence); // GPS DOP and active satellites

    // Helper methods
    double convertDMStoDD(double dms); // Convert degrees-minutes-seconds to decimal degrees
    uint8_t parseUint8(const char *str);
    uint16_t parseUint16(const char *str);
    double parseDouble(const char *str);
    bool extractField(const char *sentence, int field_num, char *output, size_t output_size);
    uint8_t calculateChecksum(const char *sentence);
    bool validateChecksum(const char *sentence);

public:
    // Constructor
    GPS();

    // Destructor
    ~GPS();

    // Initialize GPS module
    bool initialize();

    // Main loop function - call this regularly
    void update();

    // Get current GPS data
    const GPSData &getData() const { return current_data; }

    // Check if GPS has valid fix
    bool hasValidFix() const { return current_data.valid_fix; }

    // Check if new data is available
    bool isDataUpdated() const { return current_data.data_updated; }

    // Clear the data updated flag
    void clearDataUpdated() { current_data.data_updated = false; }

    // Get formatted position string
    std::string getPositionString() const;

    // Get formatted time string
    std::string getTimeString() const;

    // Get formatted date string
    std::string getDateString() const;

    // Calculate distance from previous position (in meters)
    double getDistanceFromPrevious();

    // Calculate distance between two GPS coordinates (in meters)
    static double calculateDistance(double lat1, double lon1, double lat2, double lon2);

    // Get signal strength indicator
    std::string getSignalQuality() const;

    // Send command to GPS module
    bool sendCommand(const char *command);

    // Configure GPS update rate (1-10 Hz)
    bool setUpdateRate(uint8_t rate_hz);

    // Enable/disable specific NMEA sentences
    bool enableNMEASentence(const char *sentence_type, bool enable);
};

#endif // GPS_H
