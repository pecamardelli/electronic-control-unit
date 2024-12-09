#ifndef _MAIN_H_
#define _MAIN_H_

#include "DigitalGauge.h"
#include <string>

const char *ASSETS_PATH = "./assets";
std::string IMAGES_PATH = std::string(ASSETS_PATH) + "/images";

uint8_t temp = 0;
float kml = 0;
float volts = 0;

#endif