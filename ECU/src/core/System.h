#pragma once

#include "Logger.h"
#include "helpers.h"
#include "common.h"
#include "RELAY.h"

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
extern volatile MileageData *mileage;

class System
{
private:
    /* data */
    RELAY mainRelay;
    Logger logger = Logger("System");
    ConfigMap config;
    ConfigMap defaultConfigValues = {
        {"global", {{"main_loop_interval", "50000"}, {"logo_screen_time", "2"}}},
        {"logging", {{"log_level", "INFO"}}},
        {"OF05ZAT", {{"loop_interval", "1000"}, {"pulses_per_liter", "287"}}},
        {"TempGauge", {{"loop_interval", "1000"}, {"step_offset", "-115"}}},
        {"GPS", {{"loop_interval", "1000000"}, {"baud_rate", "9600"}}},
        {"SpeedSensor", {{"loop_interval", "10"}, {"differential_pinion", "13"}, {"differential_crown", "43"}, {"tire_width", "215"}, {"aspect_ratio", "60"}, {"rim_diameter", "15"}, {"transitions_per_lap", "4"}}},
        {"Speedometer", {{"loop_interval", "1000"}, {"step_offset", "0"}}},
    };
    std::string dataPath;
    std::string totalMileageFileName;
    std::string partialMileageFileName;

    ConfigMap
    loadConfig(const std::string &);

public:
    System(std::string);
    ~System();

    void setProgramName(const char *);
    void shutdown();
    SectionMap getConfig(const std::string &);
    std::string readFile(const std::string &);
    void writeFile(const std::string &, const std::string &);
    void loadMileage();
    void saveMileage();

    static uint64_t uptime();
};