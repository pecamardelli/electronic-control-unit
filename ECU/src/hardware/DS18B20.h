/*
    Credits to Nils Henrich
    https://github.com/nilshenrich/DS18B20_cpp
*/

#pragma once

#include <string>
#include <regex>
#include <limits>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <cmath>
#include "Logger.h"

class DS18B20
{
public:
    /**
     * Empty constructor
     * Sensor address is searched automatically
     * (Only ONE sensor must be connected to raspberry pi)
     */
    DS18B20();

    /**
     * Constructor with fix sensor address
     */
    DS18B20(std::string address);

    /**
     * Empty destructor
     */
    virtual ~DS18B20();

    /**
     * Read temperature in degrees celsius
     * @return float
     */
    float readTemp();

    /**
     * Is temperature value valid?
     */
    bool isValid(float temperature);

    Logger logger = Logger("DS18B20");

private:
    // Root folder of all one-Wire devices
    static const std::string oneWireRootFolder;

    // File name of one-Wire device
    static const std::string oneWireFile;

    // Address of DS18B20 sensor (Calculated in Constructor)
    std::string address;

    // A DS18B20 cannot be copied
    DS18B20(const DS18B20 &) = delete;
    DS18B20 &operator=(const DS18B20 &) = delete;
};