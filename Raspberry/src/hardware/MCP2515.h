#pragma once

#include <iostream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h> // For byte order conversion

#include "Process.h"
#include "DBCParser.h"
#include "common.h"

class MCP2515 : public Process
{
private:
    int spi_fd = -1;                 // File descriptor for SPI
    const char *spiDevice;           // SPI device path
    const uint8_t mode = SPI_MODE_0; // SPI Mode (Mode 0 for MCP2515)
    const uint8_t bits = 8;          // Bits per word (8-bit)
    const uint32_t speed = 4000000;  // SPI Speed (4 MHz)

    bool initialized = false;
    DBCParser dbc;

public:
    MCP2515();
    ~MCP2515();

    bool begin();
    void reset();
    uint8_t spiTransfer(uint8_t data);
    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t value);
    void closeSPI();

    void parseCANMessage(struct can_frame &);
    void loop() override;
};
