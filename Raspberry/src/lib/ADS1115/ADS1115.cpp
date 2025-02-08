#include "ADS1115.h"

ADS1115::ADS1115(int _i2cBusFd) : i2cBusFd(_i2cBusFd), description("ADS1115"), logger(std::make_unique<Logger>(description))
{
}

ADS1115::~ADS1115()
{
}

int ADS1115::getRawValue(const u_int8_t channel)
{
    logger->debug("Reading raw value from channel " + std::to_string(channel) + ".");

    // Specify the I2C address of the device
    if (ioctl(i2cBusFd, I2C_SLAVE, ADS1115_ADDR) < 0)
    {
        logger->error("Failed to acquire bus access and/or talk to the slave.");
        return 0;
    }

    // Configure ADS1115 for single-ended reading on A0
    unsigned char config[3];
    config[0] = CONFIG_REG;                  // Config register address
    config[1] = 0xC2 | muxSettings[channel]; // MSB: Single-ended A0, gain ±4.096V, 128 SPS
    config[2] = 0x83;                        // LSB: Continuous conversion mode

    if (write(i2cBusFd, config, 3) != 3)
    {
        logger->error("Failed to write configuration to the I2C bus.");
        return 0;
    }

    // Read conversion result
    unsigned char reg[1] = {CONVERSION_REG};
    if (write(i2cBusFd, reg, 1) != 1)
    {
        logger->error("Failed to write register address to the I2C bus.");
        return 0;
    }

    unsigned char data[2] = {0};
    if (read(i2cBusFd, data, 2) != 2)
    {
        logger->error("Failed to read data from the I2C bus.");
        return 0;
    }

    // Combine bytes into a 16-bit value
    int rawValue = (data[0] << 8) | data[1];
    if (rawValue > 0x7FFF)
    {
        rawValue -= 0x10000; // Handle negative values (two's complement)
    }

    logger->debug("Raw value read successfully: " + std::to_string(rawValue));
    return rawValue;
}