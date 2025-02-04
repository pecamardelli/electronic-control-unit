/*
 * ADS1115.h
 *
 *  Created on: 2025-03-02
 *
 *  Analog to Digital Converter
 */

#pragma once

#include <memory>
#include "Logger.h"
#include "BCM2835Manager.h"

#define ADS1115_ADDR 0x48 // Default I2C address
#define CONVERSION_REG 0x00
#define CONFIG_REG 0x01

class ADS1115
{
private:
    /* data */
    std::string description;
    std::unique_ptr<Logger> logger;
    const unsigned char muxSettings[4] = {0x40, 0x50, 0x60, 0x70}; // A0, A1, A2, A3

public:
    ADS1115(/* args */);
    ~ADS1115();
    int getRawValue(const u_int8_t channel);
};
