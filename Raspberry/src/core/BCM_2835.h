#pragma once

#include <bcm2835.h>
#include <chrono>
#include <thread>

const std::chrono::microseconds microseconds(20);

void i2c_setSlaveAddress(uint8_t address);
uint8_t i2c_write(const char *data, uint32_t length);
uint8_t i2c_read(char *buf, uint32_t length);