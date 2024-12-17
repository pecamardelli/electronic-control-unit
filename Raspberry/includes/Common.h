#ifndef COMMON_H_
#define COMMON_H_

#include <cstdint>
#include <sys/wait.h>
#include <string>

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

#endif