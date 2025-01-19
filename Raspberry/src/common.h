#pragma once

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

#include <atomic>
#include <chrono>
#include <thread>

// Struct to store child PID and description
typedef struct _childProcess
{
    pid_t pid;
    std::string description;
} ChildProcess;

struct alignas(64) EngineValues
{
    std::atomic<float> kml;
    std::atomic<float> volts;
    std::atomic<float> fuelConsumption;
    std::atomic<bool> ignition;
};

typedef struct alignas(64) _speedSensorData
{
    uint64_t transitions;
    double speed;
    double distanceCovered;
    double averageSpeed;
} SpeedSensorData;

typedef struct alignas(64) _coolantTempSensorData
{
    float temp;
} CoolantTempSensorData;

enum Screen
{
    TORINO_LOGO,
    DIGITAL_GAUGE
};