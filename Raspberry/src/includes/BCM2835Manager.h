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

private:
    BCM2835Manager()
    {
        if (!bcm2835_init())
        {
            logger.error("BCM2835 initialization failed!");
            exit(1);
        }
        logger.info("BCM2835 initialized!");
    }

    ~BCM2835Manager()
    {
        bcm2835_close();
    }

    Logger logger = Logger("BCM2835Manager");

    // Disable copy constructor and assignment operator
    BCM2835Manager(const BCM2835Manager &) = delete;
    BCM2835Manager &operator=(const BCM2835Manager &) = delete;
};
