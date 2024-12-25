#pragma once

#include <sys/wait.h>
#include <memory> // For smart pointers
#include <iostream>
#include <vector>
#include <functional>

#include "common.h"
#include "Process.h"
#include "Logger.h"
#include "System.h"
#include "AnalogConverter.h"
#include "Relay.h"
#include "CoolantTempSensor.h"
#include "FlowSensor.h"
#include "TempGauge.h"
#include "GPS.h"
#include "helpers.h"
#include "DigitalGauge.h"
#include "SpeedSensor.h"

// Initialize static member
std::atomic<bool> Process::terminateFlag(false);

const char *ASSETS_PATH = "./src/assets";
std::string IMAGES_PATH = std::string(ASSETS_PATH) + "/images";
std::vector<ChildProcess> childProcesses; // Vector to store child processes

// Store factory functions
std::vector<std::function<std::shared_ptr<Process>()>> processFactories;

System *sys;

bool terminateProgram = false;
volatile bool terminateChildProcess = false;