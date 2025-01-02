#pragma once

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

#include <atomic>

// Struct to store child PID and description
typedef struct _childProcess
{
    pid_t pid;
    std::string description;
} ChildProcess;

struct alignas(64) EngineValues
{
    std::atomic<float> temp;
    std::atomic<float> kml;
    std::atomic<float> volts;
    std::atomic<float> fuelConsumption;
    std::atomic<bool> ignition;
    std::atomic<double> speed;
    std::atomic<double> distanceCovered;
    std::atomic<unsigned long> totalMileage;
    std::atomic<unsigned long> partialMileage;
};

enum Screen
{
    TORINO_LOGO,
    DIGITAL_GAUGE
};