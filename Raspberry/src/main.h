#pragma once

#include <sys/wait.h>
#include <sys/mman.h>
#include <signal.h>

#include "includes/Common.h"
#include "includes/Logger.h"
#include "includes/System.h"
#include "includes/AnalogConverter.h"
#include "includes/Relay.h"
#include "includes/CoolantTempSensor.h"
#include "includes/FlowSensor.h"
#include "includes/TempGauge.h"
#include "includes/GPS.h"
#include "helpers/helpers.h"
#include "RoundDisplay.h"

const char *ASSETS_PATH = "./src/assets";
std::string IMAGES_PATH = std::string(ASSETS_PATH) + "/images";
std::vector<ChildProcess> childProcesses; // Vector to store child processes

System sys;

bool terminateProgram = false;
volatile bool terminateChildProcess = false;