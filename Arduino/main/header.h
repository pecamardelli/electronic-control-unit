#ifndef _HEADER_H_
#define _HEADER_H_

#include "CircularDisplay.h"
#include "Clock.h"
#include "Commander.h"
#include "CoolantTempSensor.h"
#include "CurrentSensor.h"
#include "Flowmeter.h"
#include "Potentiometer.h"
#include "SDCard.h"
#include "SmallDisplay.h"
#include "System.h"
#include "TempGauge.h"
#include "TempSensor.h"
#include "VoltSensor.h"

float volts;
float amperes;
float coolantTemp;
int _pulses;
bool ignition = false;

int debugMode;

#endif