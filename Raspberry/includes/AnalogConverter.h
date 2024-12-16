#ifndef ANALOG_CONVERTER_H_
#define ANALOG_CONVERTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "Logger.h"

#define ADS1115_ADDR 0x48 // Default I2C address
#define CONVERSION_REG 0x00
#define CONFIG_REG 0x01

class AnalogConverter
{
private:
    /* data */
    const unsigned char muxSettings[4] = {0x40, 0x50, 0x60, 0x70}; // A0, A1, A2, A3
    int file;
    const char *FILENAME = "/dev/i2c-1";
    const u_int8_t VOLT_SENSOR_CHANNEL = 0;
    int getRawValue(const u_int8_t);
    Logger logger = Logger("AnalogConverter");

public:
    AnalogConverter(/* args */);
    ~AnalogConverter();
    float getVolts();

protected:
};

#endif