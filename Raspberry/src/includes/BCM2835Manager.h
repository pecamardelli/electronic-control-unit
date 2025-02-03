#pragma once

#include <bcm2835.h>
#include "Logger.h"

class BCM2835Manager
{
public:
    static BCM2835Manager &getInstance()
    {
        static BCM2835Manager instance; // Guaranteed to be initialized once
        return instance;
    }
    static bool ready = false;

private:
    BCM2835Manager()
    {
        if (!bcm2835_init())
        {
            logger.error("BCM2835 initialization failed!");
            exit(1);
        }

        if (!bcm2835_i2c_begin())
        {
            logger.error("Failed to initialize I2C interface.");
            exit(1);
        }

        bcm2835_i2c_set_baudrate(1000000); // Set I2C clock speed to 1mHz
        static ready = true;
        logger.info("BCM2835 initialized!");
    }

    ~BCM2835Manager()
    {
        bcm2835_i2c_end();
        bcm2835_close();
    }

    Logger logger = Logger("BCM2835Manager");

    // Disable copy constructor and assignment operator
    BCM2835Manager(const BCM2835Manager &) = delete;
    BCM2835Manager &operator=(const BCM2835Manager &) = delete;
};
