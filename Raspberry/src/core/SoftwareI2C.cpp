// SoftwareI2C.cpp
#include "SoftwareI2C.h"

SoftwareI2C::SoftwareI2C()
{
    bcm2835_gpio_fsel(SDA_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(SCL_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_set(SDA_PIN);
    bcm2835_gpio_set(SCL_PIN);
}

SoftwareI2C::~SoftwareI2C()
{
}

void SoftwareI2C::start()
{
    bcm2835_gpio_write(SDA_PIN, HIGH);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    bcm2835_gpio_write(SCL_PIN, HIGH);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    bcm2835_gpio_write(SDA_PIN, LOW);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    bcm2835_gpio_write(SCL_PIN, LOW);
}

void SoftwareI2C::stop()
{
    bcm2835_gpio_write(SDA_PIN, LOW);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    bcm2835_gpio_write(SCL_PIN, HIGH);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    bcm2835_gpio_write(SDA_PIN, HIGH);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
}

void SoftwareI2C::writeBit(uint8_t bit)
{
    bcm2835_gpio_write(SDA_PIN, bit);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    bcm2835_gpio_write(SCL_PIN, HIGH);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    bcm2835_gpio_write(SCL_PIN, LOW);
}

uint8_t SoftwareI2C::readBit()
{
    bcm2835_gpio_fsel(SDA_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_write(SCL_PIN, HIGH);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    uint8_t bit = bcm2835_gpio_lev(SDA_PIN);
    bcm2835_gpio_write(SCL_PIN, LOW);
    std::this_thread::sleep_for(std::chrono::nanoseconds(I2C_DELAY_NS));
    bcm2835_gpio_fsel(SDA_PIN, BCM2835_GPIO_FSEL_OUTP);
    return bit;
}

uint8_t SoftwareI2C::writeByte(uint8_t byte)
{
    for (int i = 0; i < 8; i++)
    {
        writeBit((byte & 0x80) != 0);
        byte <<= 1;
    }
    return readBit(); // Read ACK
}

uint8_t SoftwareI2C::readByte(uint8_t ack)
{
    uint8_t byte = 0;
    for (int i = 0; i < 8; i++)
    {
        byte = (byte << 1) | readBit();
    }
    writeBit(!ack);
    return byte;
}
