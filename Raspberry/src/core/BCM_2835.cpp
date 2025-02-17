#include "BCM_2835.h"

void i2c_setSlaveAddress(uint8_t address)
{
    std::this_thread::sleep_for(microseconds);
    bcm2835_i2c_setSlaveAddress(address);
}

uint8_t i2c_write(const char *buf, uint32_t len)
{
    std::this_thread::sleep_for(microseconds);
    return bcm2835_i2c_write(buf, len);
}

uint8_t i2c_read(char *buf, uint32_t len)
{
    std::this_thread::sleep_for(microseconds);
    return bcm2835_i2c_read(buf, len);
}
