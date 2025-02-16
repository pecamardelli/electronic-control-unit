#pragma once

#include <string>

#ifndef CHILD_PROCESS_H_
#define CHILD_PROCESS_H_
typedef struct _childProcess
{
    pid_t pid;
    std::string description;
} ChildProcess;
#endif

#ifndef ENGINE_VALUES_H_
#define ENGINE_VALUES_H_
typedef struct alignas(64) _engineValues
{
    double kml;
    double volts;
    bool ignition;
} EngineValues;
#endif

#ifndef SPEED_SENSOR_DATA_H_
#define SPEED_SENSOR_DATA_H_
typedef struct alignas(64) _speedSensorData
{
    uint64_t transitions;
    double speed;
    double distanceCovered;
    double averageSpeed;
} SpeedSensorData;
#endif

#ifndef COOLANT_TEMP_SENSOR_DATA_H_
#define COOLANT_TEMP_SENSOR_DATA_H_
typedef struct alignas(64) _coolantTempSensorData
{
    float temp;
} CoolantTempSensorData;
#endif

#ifndef FUEL_CONSUMPTION_DATA_H_
#define FUEL_CONSUMPTION_DATA_H_
typedef struct alignas(64) _fuelConsumptionData
{
    double fuelConsumption;
} FuelConsumptionData;
#endif

enum Screen
{
    TORINO_LOGO,
    DIGITAL_GAUGE
};