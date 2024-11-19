#include "VoltSensor.h"

VoltSensor::VoltSensor(/* args */) {}

VoltSensor::~VoltSensor() {}

float VoltSensor::getMeasure() {
  // Read the Analog Input
  adcValue = analogRead(VOLT_SENSOR_INPUT_PIN);

  // Determine voltage at ADC input
  adcVoltage = (adcValue * referenceVoltage) / 1024.0;

  // Calculate voltage at divider input
  inVoltage = adcVoltage / (R2 / (R1 + R2));

  return inVoltage;
}
