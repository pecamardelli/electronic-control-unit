#include "AnalogConverter.h"
#include "Logger.h"

AnalogConverter::AnalogConverter(/* args */)
{
    info("Initializing AnalogConverter.");
    className = "AnalogConverter";

    // Open I2C bus
    if ((file = open(FILENAME, O_RDWR)) < 0)
    {
        error("Failed to open the I2C bus.");
    }
    else
    {
        info("I2C bus opened successfully.");
    }

    // Specify the I2C address of the device
    if (ioctl(file, I2C_SLAVE, ADS1115_ADDR) < 0)
    {
        error("Failed to acquire bus access and/or talk to the slave.");
    }
    else
    {
        info("I2C device address set successfully.");
    }
}

AnalogConverter::~AnalogConverter()
{
    info("Closing I2C bus.");
    // Close I2C file
    close(file);
}

int AnalogConverter::getRawValue(const u_int8_t channel)
{
    debug("Reading raw value from channel " + std::to_string(channel) + ".");

    // Configure ADS1115 for single-ended reading on A0
    unsigned char config[3];
    config[0] = CONFIG_REG;                  // Config register address
    config[1] = 0xC2 | muxSettings[channel]; // MSB: Single-ended A0, gain ±4.096V, 128 SPS
    config[2] = 0x83;                        // LSB: Continuous conversion mode

    if (write(file, config, 3) != 3)
    {
        error("Failed to write configuration to the I2C bus.");
        return 0;
    }

    // Read conversion result
    unsigned char reg[1] = {CONVERSION_REG};
    if (write(file, reg, 1) != 1)
    {
        error("Failed to write register address to the I2C bus.");
        return 0;
    }

    unsigned char data[2] = {0};
    if (read(file, data, 2) != 2)
    {
        error("Failed to read data from the I2C bus.");
        return 0;
    }

    // Combine bytes into a 16-bit value
    int rawValue = (data[0] << 8) | data[1];
    if (rawValue > 0x7FFF)
    {
        rawValue -= 0x10000; // Handle negative values (two's complement)
    }

    debug("Raw value read successfully: " + std::to_string(rawValue));
    return rawValue;
}

float AnalogConverter::getVolts()
{
    debug("Calculating voltage from raw value.");

    int rawValue = getRawValue(VOLT_SENSOR_CHANNEL);
    // Calculate voltage (assuming gain ±4.096V, resolution = 16-bit)
    // Scaling factor for MH 25V sensor is 5
    float volts = rawValue * 4.096 / 32768.0 * 5;

    debug("Voltage calculated: " + std::to_string(volts));

    return volts >= 0 ? volts : 0;
}
