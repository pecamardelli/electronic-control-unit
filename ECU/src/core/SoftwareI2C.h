// SoftwareI2C.h
#pragma once

#include <bcm2835.h>
#include <iostream>
#include <unistd.h>
#include <thread>

constexpr int I2C_DELAY_NS = 1;
#define SDA_PIN RPI_V2_GPIO_P1_08
#define SCL_PIN RPI_V2_GPIO_P1_10

class SoftwareI2C
{
public:
    SoftwareI2C();
    ~SoftwareI2C();
    void start();
    void stop();
    void writeBit(uint8_t bit);
    uint8_t readBit();
    uint8_t writeByte(uint8_t byte);
    uint8_t readByte(uint8_t ack);

private:
};