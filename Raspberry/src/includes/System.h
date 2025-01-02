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

using SectionMap = std::map<std::string, std::string>;
using ConfigMap = std::map<std::string, SectionMap>;
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
        {"SpeedSensor", {{"loop_interval", "10"}, {"differential_pinion", "13"}, {"differential_crown", "43"}, {"tire_width", "215"}, {"aspect_ratio", "60"}, {"rim_diameter", "15"}, {"transitions_per_lap", "4"}}},
        {"Speedometer", {{"loop_interval", "1000"}, {"step_offset", "0"}}},
    };
    std::string dataPath;
    std::ifstream totalMileage;

    ConfigMap
    loadConfig(const std::string &);

public:
    System(std::string);
    ~System();

    void setProgramName(const char *);
    void shutdown();
    SectionMap getConfig(const std::string &);
    uint64_t getTotalMileage();

    static uint64_t uptime();
};