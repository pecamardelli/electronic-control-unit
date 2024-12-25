#pragma once

#include "Logger.h"
#include "Relay.h"
#include "../helpers/helpers.h"

#ifndef MAP_H_
#define MAP_H_
#include <map>
#endif

#ifndef SSTREAM_H_
#define SSTREAM_H_
#include <sstream>
#endif

#include <cstring>

using ConfigMap = std::map<std::string, std::map<std::string, std::string>>;
extern char *programName;

class System
{
private:
    /* data */
    Relay mainRelay;
    Logger logger = Logger("System");
    ConfigMap config;
    ConfigMap defaultConfigValues = {
        {"global", {{"main_loop_interval", "50000"}, {"logo_screen_time", "2"}}},
        {"logging", {{"log_level", "INFO"}}},
        {"FlowSensor", {{"loop_interval", "1000"}, {"pulses_per_liter", "287"}}},
        {"TempGauge", {{"loop_interval", "1000"}, {"step_offset", "-115"}}},
        {"GPS", {{"loop_interval", "1000000"}, {"baud_rate", "9600"}}},
        {"SpeedSensor", {{"loop_interval", "10"}}}};

    ConfigMap loadConfig(const std::string &);

public:
    System(std::string);
    ~System();

    void setProgramName(const char *);
    void shutdown();
    static uint64_t uptime();

    // Implementing this template method in here in order to avoid linkage issues.
    template <typename T>
    T getConfigValue(const std::string &section, const std::string &key) const
    {
        // Locate the section
        auto sectionIt = config.find(section);
        if (sectionIt == config.end())
        {
            throw std::runtime_error("Section not found: " + section);
        }

        // Locate the key within the section
        auto keyIt = sectionIt->second.find(key);
        if (keyIt == sectionIt->second.end())
        {
            throw std::runtime_error("Key not found in section [" + section + "]: " + key);
        }

        // Retrieve the value as a string
        const std::string &valueStr = keyIt->second;
        // Convert the string value to the requested type
        if constexpr (std::is_same<T, bool>::value)
        {
            if (valueStr == "1" || valueStr == "true" || valueStr == "TRUE" || valueStr == "True")
            {
                return true;
            }
            else if (valueStr == "0" || valueStr == "false" || valueStr == "FALSE" || valueStr == "False")
            {
                return false;
            }
            else
            {
                throw std::runtime_error("Conversion failed for key [" + key + "] in section [" + section + "]: Invalid boolean value " + valueStr);
            }
        }
        else
        {
            std::istringstream iss(valueStr);
            T convertedValue;
            if (!(iss >> convertedValue))
            {
                throw std::runtime_error("Conversion failed for key [" + key + "] in section [" + section + "]: " + valueStr);
            }
            return convertedValue;
        }
    }
};