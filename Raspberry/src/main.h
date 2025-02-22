#pragma once

#define I2C_BUS "/dev/i2c-1"

#include <sys/wait.h>
#include <memory> // For smart pointers
#include <iostream>
#include <vector>
#include <functional>
#include <typeinfo>
#include <bcm2835.h>
#include <atomic>
#include <chrono>
#include <thread>

#include "DS18B20.h"
#include "ADS1115.h"
#include "GPS.h"
#include "TCA9548A.h"
#include "DHT11.h"
#include "RELAY.h"

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

volatile EngineValues *engineValues = nullptr;
volatile SpeedSensorData *speedSensorData = nullptr;
volatile CoolantTempSensorData *coolantTempSensorData = nullptr;
volatile MileageData *mileage = nullptr;

System *sys;
std::unique_ptr<ADS1115> ads1115;

bool terminateProgram = false;
volatile bool terminateChildProcess = false;