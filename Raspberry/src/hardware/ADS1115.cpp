#include "ADS1115.h"

ADS1115::ADS1115()
    : description("ADS1115"), logger(std::make_unique<Logger>(description))
{
}

ADS1115::~ADS1115()
{
}

int ADS1115::getRawValue(const uint8_t channel)
{
    logger->debug("Reading raw value from channel " + std::to_string(channel) + ".");

    i2c_setSlaveAddress(ADS1115_ADDR);

    unsigned char config[3];
    config[0] = CONFIG_REG;
    config[1] = 0xC2 | muxSettings[channel]; // MSB: Single-ended, gain ±4.096V, 128 SPS
    config[2] = 0x83;                        // LSB: Continuous conversion mode

    if (i2c_write(reinterpret_cast<char *>(config), 3) != BCM2835_I2C_REASON_OK)
    {
        logger->error("Failed to write configuration to ADS1115.");
        return 0;
    }

    // Read conversion result
    unsigned char reg = CONVERSION_REG;
    if (i2c_write(reinterpret_cast<char *>(&reg), 1) != BCM2835_I2C_REASON_OK)
    {
        logger->error("Failed to set conversion register.");
        return 0;
    }

    unsigned char data[2] = {0};
    if (i2c_read(reinterpret_cast<char *>(data), 2) != BCM2835_I2C_REASON_OK)
    {
        logger->error("Failed to read data from ADS1115.");
        return 0;
    }

    int rawValue = (data[0] << 8) | data[1];
    if (rawValue > 0x7FFF)
    {
        rawValue -= 0x10000; // Handle negative values (two's complement)
    }

    logger->debug("Raw value read successfully: " + std::to_string(rawValue));
    return rawValue;
}
