#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include <iostream>
#include <ctime>
#include <string>
#include <fstream>
#include <map>

#include "Logger.h"
#include "Relay.h"
#include "../helpers/helpers.h"

using ConfigMap = std::map<std::string, std::map<std::string, std::string>>;

class System
{
private:
    /* data */
    Relay mainRelay;
    Logger logger = Logger("System");
    const std::string configFile = "config.ini";
    ConfigMap loadConfig(const std::string &);
    ConfigMap defaultConfigValues = {
        {"global", {{"hostname", "torino-ecu"}, {"main_loop_rate", "5000"}}},
        {"FlowSensor", {{"flow_sensor_loop_rate", "50000"}}},
        {"logging", {{"log_level", "INFO"}}}};

public:
    System(/* args */);
    ~System();
    useconds_t flowSensorLoopRate;
    useconds_t mainProgramLoopRate;
    ConfigMap config;

    void shutdown();
    static uint64_t uptime();
};

#endif