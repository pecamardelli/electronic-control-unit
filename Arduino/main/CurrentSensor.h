/**************************************************************************/
/*!
  @file     CurrentSensor.h

  Original library by Pablin defining the logic for the voltage sensor.

  License: MIT (see LICENSE)
*/
/**************************************************************************/

#ifndef _CURRENTSENSOR_H
#define _CURRENTSENSOR_H

#include "common.h"

class CurrentSensor {
private:
  int pin = A2;
  // Floats for ADC voltage & Input voltage
  float adcVoltage = 0.0;
  float inVoltage = 0.0;

  // Float for Reference Voltage
  const float referenceVoltage = 5.0;
  const float lowerLimit = -100.0;
  const float upperLimit = 100.0;
  const float range = upperLimit - lowerLimit;
  const float voltToAmperRatio = range / referenceVoltage;

  float errorAdjustment = 0.98;

  // Integer for ADC value
  int adcValue = 0;

public:
  CurrentSensor(/* args */);
  ~CurrentSensor();
  float getMeasure();
};

#endif  // _CURRENTSENSOR_H
