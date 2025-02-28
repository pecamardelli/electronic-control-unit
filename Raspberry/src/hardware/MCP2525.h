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

#include "Base.h"
#include "DBCParser.h"

class MCP2515 : public Base
{
private:
    int spi_fd;            // File descriptor for SPI
    const char *spiDevice; // SPI device path
    uint8_t mode;          // SPI Mode (Mode 0 for MCP2515)
    uint8_t bits;          // Bits per word (8-bit)
    uint32_t speed;        // SPI Speed (4 MHz)

    bool initialized = false;
    DBCParser dbc;

public:
    MCP2515(const char *, uint32_t);
    ~MCP2515();

    bool begin();
    void reset();
    uint8_t spiTransfer(uint8_t data);
    uint8_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint8_t value);
    void closeSPI();

    void parseCANMessage(struct can_frame &);
    void receiveLiveData();
};
