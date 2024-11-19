#include "CurrentSensor.h"

CurrentSensor::CurrentSensor(/* args */) {}

CurrentSensor::~CurrentSensor() {}

float CurrentSensor::getMeasure() {
  // Read the Analog Input
  adcValue = analogRead(pin);

  // Determine voltage at ADC input
  adcVoltage = (adcValue * referenceVoltage) / 1024.0;

  return lowerLimit + voltToAmperRatio * adcVoltage + errorAdjustment;
}
