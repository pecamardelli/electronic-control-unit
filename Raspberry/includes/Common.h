#ifndef COMMON_H_
#define COMMON_H_

#include <cstdint>
#include <sys/wait.h>
#include <string>
#include <map>
#include <variant>
#include <sys/mman.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include <iostream>
#include <ctime>
#include <fstream>

// Struct to store child PID and description
typedef struct _childProcess
{
    pid_t pid;
    std::string description;
} ChildProcess;

typedef struct _engineValues
{
    uint8_t temp;
    float kml;
    float volts;
    float fuelConsumption;
    bool ignition;
} EngineValues;

enum Screen
{
    TORINO_LOGO,
    DIGITAL_GAUGE
};

// Config values: using 'variant' to hold different types.
using ConfigValue = std::variant<int, float, bool, std::string>;
using ConfigMap = std::map<std::string, std::map<std::string, ConfigValue>>;

#endif