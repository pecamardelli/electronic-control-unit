#include "Ads1115.h"

Ads1115::Ads1115(/* args */)
{
    // Open I2C bus
    if ((file = open(FILENAME, O_RDWR)) < 0)
    {
        perror("Failed to open the I2C bus");
    }

    // Specify the I2C address of the device
    if (ioctl(file, I2C_SLAVE, ADS1115_ADDR) < 0)
    {
        perror("Failed to acquire bus access and/or talk to the slave");
    }
}

Ads1115::~Ads1115()
{
    // Close I2C file
    close(file);
}

int Ads1115::getRawValue(const u_int8_t channel)
{
    // Configure ADS1115 for single-ended reading on A0
    unsigned char config[3];
    config[0] = CONFIG_REG;                  // Config register address
    config[1] = 0xC2 | muxSettings[channel]; // MSB: Single-ended A0, gain ±4.096V, 128 SPS
    config[2] = 0x83;                        // LSB: Continuous conversion mode

    if (write(file, config, 3) != 3)
    {
        perror("Failed to write to the i2c bus");
        return 0;
    }

    // Commenting this out because the main loop runs every 50 ms
    // Wait for conversion
    // usleep(8000); // ADS1115 takes ~8ms at 128 SPS

    // Read conversion result
    unsigned char reg[1] = {CONVERSION_REG};
    if (write(file, reg, 1) != 1)
    {
        perror("Failed to write to the i2c bus");
        return 0;
    }

    unsigned char data[2] = {0};
    if (read(file, data, 2) != 2)
    {
        perror("Failed to read from the i2c bus");
        return 0;
    }

    // Combine bytes into a 16-bit value
    int rawValue = (data[0] << 8) | data[1];
    if (rawValue > 0x7FFF)
    {
        rawValue -= 0x10000; // Handle negative values (two's complement)
    }

    return rawValue;
}

float Ads1115::getVolts()
{
    int rawValue = getRawValue(VOLT_SENSOR_CHANNEL);
    // Calculate voltage (assuming gain ±4.096V, resolution = 16-bit)
    // Scaling factor for MH 25V sensor is 5
    float volts = rawValue * 4.096 / 32768.0 * 5;

    printf("Channel %d Raw Value: %d\n", VOLT_SENSOR_CHANNEL, rawValue);
    printf("Voltage: %.1f V\n", volts);

    return volts >= 0 ? volts : 0;
}