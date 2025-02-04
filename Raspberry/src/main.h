#pragma once

#include <sys/wait.h>
#include <memory> // For smart pointers
#include <iostream>
#include <vector>
#include <functional>
#include <typeinfo>

#include "common.h"
#include "Process.h"
#include "Logger.h"
#include "System.h"
#include "Relay.h"
#include "CoolantTempSensor.h"
#include "FlowSensor.h"
#include "TempGauge.h"
#include "GPS.h"
#include "helpers.h"
#include "DigitalGauge.h"
#include "SpeedSensor.h"
#include "Speedometer.h"
#include "I2CMultiplexer.h"
#include "TempSensor.h"
#include "ADS1115.h"
#include "VoltSensor.h"

// Initialize static member
std::atomic<bool> Process::terminateFlag(false);

const char *ASSETS_PATH = "./src/assets";
std::string IMAGES_PATH = std::string(ASSETS_PATH) + "/images";
std::vector<ChildProcess> childProcesses; // Vector to store child processes

struct Factory
{
    std::string typeName;
    std::function<std::shared_ptr<Process>()> create;
};

// Store factory functions
std::vector<Factory> processFactories;

volatile SpeedSensorData *speedSensorData = nullptr;
volatile CoolantTempSensorData *coolantTempSensorData = nullptr;

System *sys;
ADS1115 ads1115;

bool terminateProgram = false;
volatile bool terminateChildProcess = false;