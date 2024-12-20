#pragma once

#include "Logger.h"
#include "Relay.h"
#include "../helpers/helpers.h"

#ifndef SSTREAM_H_
#define SSTREAM_H_
#include <sstream>
#endif

using ConfigMap = std::map<std::string, std::map<std::string, std::string>>;
class System
{
private:
    /* data */
    Relay mainRelay;
    Logger logger = Logger("System");
    const std::string configFile = "config.ini";
    ConfigMap config;
    ConfigMap defaultConfigValues = {
        {"global", {{"hostname", "torino-ecu"}, {"main_loop_rate", "100000"}}},
        {"FlowSensor", {{"flow_sensor_loop_rate", "1000"}}},
        {"logging", {{"log_level", "INFO"}}}};

    ConfigMap loadConfig(const std::string &);

public:
    System(/* args */);
    ~System();

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
        std::istringstream iss(valueStr);
        T convertedValue;
        if (!(iss >> convertedValue))
        {
            throw std::runtime_error("Conversion failed for key [" + key + "] in section [" + section + "]: " + valueStr);
        }

        return convertedValue;
    }
};