/**************************************************************************/
/*!
  @file     VoltSensor.h

  Original library by Pablin defining the logic for the voltage sensor.

  License: MIT (see LICENSE)
*/
/**************************************************************************/

#ifndef _VOLTSENSOR_H
#define _VOLTSENSOR_H

#include "common.h"

#define VOLT_SENSOR_INPUT_PIN A7

class VoltSensor {
 private:
  // Floats for ADC voltage & Input voltage
  float adcVoltage = 0.0;
  float inVoltage = 0.0;

  // Floats for resistor values in divider (in ohms)
  const float R1 = 30000.0;
  const float R2 = 7500.0;

  // Float for Reference Voltage
  const float referenceVoltage = 5.0;

  // Integer for ADC value
  int adcValue = 0;

 public:
  VoltSensor(/* args */);
  ~VoltSensor();
  float getMeasure();
};

#endif  // _VOLTSENSOR_H
