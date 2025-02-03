#include "AnalogConverter.h"
#include "Logger.h"

AnalogConverter::AnalogConverter(/* args */)
{
    logger.info("Initializing AnalogConverter.");
    BCM2835Manager::getInstance();

    bcm2835_i2c_setSlaveAddress(ADS1115_ADDR);
}

AnalogConverter::~AnalogConverter()
{
}

int AnalogConverter::getRawValue(const u_int8_t channel)
{
    logger.debug("Reading raw value from channel " + std::to_string(channel) + ".");

    unsigned char config[3];
    config[0] = CONFIG_REG;
    config[1] = 0xC2 | muxSettings[channel]; // Configuring ADS1115
    config[2] = 0x83;

    if (bcm2835_i2c_write(reinterpret_cast<char *>(config), 3) != BCM2835_I2C_REASON_OK)
    {
        logger.error("Failed to write configuration to ADS1115.");
        return 0;
    }

    unsigned char reg = CONVERSION_REG;
    if (bcm2835_i2c_write(reinterpret_cast<char *>(&reg), 1) != BCM2835_I2C_REASON_OK)
    {
        logger.error("Failed to write register address to ADS1115.");
        return 0;
    }

    unsigned char data[2] = {0};
    if (bcm2835_i2c_read(reinterpret_cast<char *>(data), 2) != BCM2835_I2C_REASON_OK)
    {
        logger.error("Failed to read data from ADS1115.");
        return 0;
    }

    int rawValue = (data[0] << 8) | data[1];
    if (rawValue > 0x7FFF)
    {
        rawValue -= 0x10000; // Handle negative values
    }

    logger.debug("Raw value read successfully: " + std::to_string(rawValue));
    return rawValue;
}

float AnalogConverter::getVolts()
{
    logger.debug("Calculating voltage from raw value.");

    int rawValue = getRawValue(VOLT_SENSOR_CHANNEL);
    float volts = rawValue * 4.096 / 32768.0 * 5; // Scaling factor for MH 25V sensor

    logger.debug("Voltage calculated: " + std::to_string(volts));

    return volts >= 0 ? volts : 0;
}