#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sstream>

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

using ConfigMap = std::map<std::string, std::map<std::string, std::string>>;