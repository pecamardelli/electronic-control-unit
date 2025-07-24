#include "PicoComm.h"
#include <cstring>
#include <stdio.h>

// Simple data packet format constants
#define PACKET_START_BYTE    0xAA
#define PACKET_END_BYTE      0x55
#define PACKET_DATA_SIZE     12  // 4 bytes speed + 3 bytes time + 1 byte flags + 4 bytes timestamp
#define PACKET_TOTAL_SIZE    15  // start + data + checksum + end

PicoComm::PicoComm() {
    uart_instance = nullptr;
    
    tx_pin = 4;  // Default UART1 pins for GPS communication
    rx_pin = 5;
    
    baudrate = 115200;
    
    buffer_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
    
    // Initialize data structure
    current_data.speed = 0.0f;
    current_data.hours = 0;
    current_data.minutes = 0;
    current_data.seconds = 0;
    current_data.valid = false;
    current_data.timestamp = 0;
}

bool PicoComm::configureUART(uart_inst_t* uart, uint tx, uint rx, uint baud) {
    uart_instance = uart;
    tx_pin = tx;
    rx_pin = rx;
    baudrate = baud;
    return true;
}

bool PicoComm::begin() {
    if (!uart_instance) return false;
    
    // Initialize UART
    uart_init(uart_instance, baudrate);
    
    // Set the GPIO pin mux to the UART
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);
    
    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(uart_instance, false, false);
    
    // Set data format: 8 data bits, 1 stop bit, no parity
    uart_set_format(uart_instance, 8, 1, UART_PARITY_NONE);
    
    // Turn off FIFO's for character-by-character processing
    uart_set_fifo_enabled(uart_instance, false);
    
    return true;
}

bool PicoComm::hasNewData() {
    if (!uart_instance) return false;
    
    while (uart_is_readable(uart_instance)) {
        uint8_t byte = uart_getc(uart_instance);
        
        // Look for start byte
        if (buffer_index == 0 && byte == PACKET_START_BYTE) {
            rx_buffer[buffer_index++] = byte;
        }
        // Collect data if we've started
        else if (buffer_index > 0 && buffer_index < sizeof(rx_buffer)) {
            rx_buffer[buffer_index++] = byte;
            
            // Check if we have a complete packet
            if (buffer_index >= PACKET_TOTAL_SIZE) {
                if (rx_buffer[PACKET_TOTAL_SIZE - 1] == PACKET_END_BYTE) {
                    // Valid packet received
                    bool success = parseSpeedometerData(rx_buffer, PACKET_TOTAL_SIZE);
                    clearBuffer();
                    return success;
                } else {
                    // Invalid packet, reset
                    clearBuffer();
                }
            }
        }
        else {
            // Buffer overflow or invalid state, reset
            clearBuffer();
        }
    }
    
    return false;
}

bool PicoComm::parseSpeedometerData(const uint8_t* data, uint16_t length) {
    if (length < PACKET_TOTAL_SIZE) return false;
    
    // Verify start and end bytes
    if (data[0] != PACKET_START_BYTE || data[PACKET_TOTAL_SIZE - 1] != PACKET_END_BYTE) {
        return false;
    }
    
    // Verify checksum
    if (!validateChecksum(data, length)) {
        return false;
    }
    
    // Extract data (skip start byte)
    const uint8_t* payload = &data[1];
    
    // Parse speed (4 bytes, little endian float)
    memcpy(&current_data.speed, payload, 4);
    
    // Parse time (3 bytes)
    current_data.hours = payload[4];
    current_data.minutes = payload[5];
    current_data.seconds = payload[6];
    
    // Parse flags (1 byte)
    uint8_t flags = payload[7];
    current_data.valid = (flags & 0x01) != 0;
    
    // Parse timestamp (4 bytes, little endian) - not used but skip it
    // uint32_t remote_timestamp;
    // memcpy(&remote_timestamp, &payload[8], 4);
    
    // Set local timestamp
    current_data.timestamp = to_ms_since_boot(get_absolute_time());
    
    return true;
}

bool PicoComm::validateChecksum(const uint8_t* data, uint16_t length) {
    if (length < 3) return false;  // Need at least start, checksum, end
    
    uint8_t calculated_checksum = 0;
    
    // Calculate checksum of data portion (exclude start, checksum, and end bytes)
    for (int i = 1; i < length - 2; i++) {
        calculated_checksum ^= data[i];
    }
    
    uint8_t received_checksum = data[length - 2];
    return calculated_checksum == received_checksum;
}

bool PicoComm::readSpeedometerData(SpeedometerData& data) {
    data = current_data;
    return current_data.valid;
}

SpeedometerData PicoComm::getLatestData() {
    return current_data;
}

bool PicoComm::isConnected() {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    uint32_t time_since_last_data = current_time - current_data.timestamp;
    
    // Consider disconnected if no data for more than 5 seconds
    return time_since_last_data < 5000;
}

uint32_t PicoComm::getLastDataTime() {
    return current_data.timestamp;
}

void PicoComm::clearBuffer() {
    buffer_index = 0;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

std::string PicoComm::formatSpeed(float speed, bool metric) {
    char buffer[32];
    if (metric) {
        snprintf(buffer, sizeof(buffer), "%.1f km/h", speed);
    } else {
        snprintf(buffer, sizeof(buffer), "%.1f mph", speed);
    }
    return std::string(buffer);
}

std::string PicoComm::formatTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d", hours, minutes, seconds);
    return std::string(buffer);
}
