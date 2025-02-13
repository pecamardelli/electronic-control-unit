#include "I2CMultiplexer.h"

I2CMultiplexer::I2CMultiplexer(/* args */)
{
    logger = std::make_unique<Logger>(description);
}

I2CMultiplexer::~I2CMultiplexer()
{
}

// Function to select a channel on the TCA9548A
void I2CMultiplexer::selectChannel(uint8_t channel)
{
    if (channel > 7)
    {
        logger->error("Invalid channel number. Must be between 0 and 7.");
        return;
    }

    bcm2835_i2c_setSlaveAddress(TCA9548A_ADDRESS);

    char data = 1 << channel; // Bitmask for the selected channel
    if (bcm2835_i2c_write(&data, 1))
    {
        logger->error("Failed to select channel.");
    }
}