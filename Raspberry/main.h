#ifndef _MAIN_H_
#define _MAIN_H_

#include "includes/System.h"
#include "includes/Ads1115.h"
#include "includes/Relay.h"
#include "helpers/helpers.h"
#include "DigitalGauge.h"

#include <string>

const char *ASSETS_PATH = "./assets";
std::string IMAGES_PATH = std::string(ASSETS_PATH) + "/images";

uint8_t temp = 0;
float kml = 0;
float volts = 0;
bool ignition = false;

#endif