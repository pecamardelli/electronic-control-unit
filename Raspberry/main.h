#ifndef _MAIN_H_
#define _MAIN_H_

#include "includes/Common.h"
#include "includes/Logger.h"
#include "includes/System.h"
#include "includes/AnalogConverter.h"
#include "includes/Relay.h"
#include "includes/CoolantTempSensor.h"
#include "includes/FlowSensor.h"
#include "helpers/helpers.h"
#include "RoundDisplay.h"

#include <string>
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>

const char *ASSETS_PATH = "./assets";
std::string IMAGES_PATH = std::string(ASSETS_PATH) + "/images";

EngineValues engineValues;

bool terminateProgram = false;

#endif