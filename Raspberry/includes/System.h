#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "Common.h"

#include "Logger.h"
#include "Relay.h"
#include "../helpers/helpers.h"

class System
{
private:
    /* data */
    Relay mainRelay;
    Logger logger = Logger("System");
    const std::string configFile = "config.ini";
    ConfigMap defaultConfigValues = {
        {"global", {{"hostname", "torino-ecu"}, {"main_loop_rate", "100000"}}},
        {"FlowSensor", {{"flow_sensor_loop_rate", "1000"}}},
        {"logging", {{"log_level", "INFO"}}}};

    ConfigMap loadConfig(const std::string &);

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