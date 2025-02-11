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

class CoolantTempSensor
{
public:
    /**
     * Empty constructor
     * Sensor address is searched automatically
     * (Only ONE sensor must be connected to raspberry pi)
     */
    CoolantTempSensor();

    /**
     * Constructor with fix sensor address
     */
    CoolantTempSensor(std::string address);

    /**
     * Empty destructor
     */
    virtual ~CoolantTempSensor();

    /**
     * Read temperature in degrees celsius
     * @return float
     */
    float readTemp();

    /**
     * Is temperature value valid?
     */
    bool isValid(float temperature);

    Logger logger = Logger("CoolantTempSensor");

private:
    // Root folder of all one-Wire devices
    static const std::string oneWireRootFolder;

    // File name of one-Wire device
    static const std::string oneWireFile;

    // Address of DS18B20 sensor (Calculated in Constructor)
    std::string address;

    // A DS18B20 cannot be copied
    CoolantTempSensor(const CoolantTempSensor &) = delete;
    CoolantTempSensor &operator=(const CoolantTempSensor &) = delete;
};