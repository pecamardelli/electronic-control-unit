#ifndef PICO_COMM_H
#define PICO_COMM_H

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include <string>

// Data structure for GPS speedometer data
struct SpeedometerData {
    float speed;           // Speed in km/h or mph
    uint8_t hours;         // GPS time
    uint8_t minutes;
    uint8_t seconds;
    bool valid;            // Data validity flag
    uint32_t timestamp;    // Local timestamp when data was received
};

class PicoComm {
private:
    uart_inst_t* uart_instance;
    
    // Pin configurations
    uint tx_pin, rx_pin;     // UART pins
    
    // Communication parameters
    uint baudrate;
    
    // Data buffer
    uint8_t rx_buffer[64];   // Simplified smaller buffer
    uint16_t buffer_index;
    
    // Current data
    SpeedometerData current_data;
    
    // Private methods
    bool parseSpeedometerData(const uint8_t* data, uint16_t length);
    bool validateChecksum(const uint8_t* data, uint16_t length);

public:
    // Constructor
    PicoComm();
    
    // Simple UART configuration
    bool configureUART(uart_inst_t* uart, uint tx, uint rx, uint baud = 115200);
    
    // Initialize communication
    bool begin();
    
    // Data reception methods
    bool hasNewData();
    bool readSpeedometerData(SpeedometerData& data);
    SpeedometerData getLatestData();
    
    // Communication status
    bool isConnected();
    uint32_t getLastDataTime();
    
    // Utility methods
    void clearBuffer();
    
    // Data formatting helpers
    std::string formatSpeed(float speed, bool metric = true);
    std::string formatTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
};

#endif // PICO_COMM_H
