#pragma once

#include <functional>

#include "ADS1115.h"
#include "DHT11.h"
#include "DS3231.h"

#include "common.h"
#include "Process.h"
#include "Logger.h"
#include "System.h"
#include "TempGauge.h"
#include "helpers.h"
#include "DigitalGauge.h"
#include "SpeedSensor.h"
#include "Speedometer.h"
#include "VoltSensor.h"
#include "SSD1306Software.h"
#include "SSD1306Hardware.h"

// Initialize static member
std::atomic<bool> Process::terminateFlag(false);

std::vector<ChildProcess> childProcesses; // Vector to store child processes

struct Factory
{
    std::string typeName;
    std::function<std::shared_ptr<Process>()> create;
};

// Store factory functions
std::vector<Factory> processFactories;

volatile EngineValues *engineValues = nullptr;
volatile SpeedSensorData *speedSensorData = nullptr;
volatile CoolantTempSensorData *coolantTempSensorData = nullptr;
volatile MileageData *mileage = nullptr;

System *sys;
std::unique_ptr<ADS1115> ads1115;

bool terminateProgram = false;
volatile bool terminateChildProcess = false;