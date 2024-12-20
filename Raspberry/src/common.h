#pragma once

#ifndef MAP_H_
#define MAP_H_
#include <map>
#endif

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

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