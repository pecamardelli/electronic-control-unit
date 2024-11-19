#include "CoolantTempSensor.h"

OneWire ourWire(2);
DallasTemperature sensors(&ourWire);

CoolantTempSensor::CoolantTempSensor(/* args */) {}

CoolantTempSensor::~CoolantTempSensor() {}

void CoolantTempSensor::setup() {
  Serial.println("Coolant Temperature Sensor initiating...");
  Serial.print("Library Version: ");
  Serial.println(DALLASTEMPLIBVERSION);

  sensors.begin();
  sensors.getAddress(tempDeviceAddress, 0);
  sensors.setResolution(tempDeviceAddress, resolution);
  sensors.setWaitForConversion(false);
  sensors.requestTemperatures();
  delayInMillis = 750 / (1 << (12 - resolution));
  lastTempRequest = millis();
  inited = true;
}

float CoolantTempSensor::getMeasure() {
  if (!inited) setup();

  if (millis() - lastTempRequest >= delayInMillis)  // waited long enough??
  {
    temperature = sensors.getTempCByIndex(0);
    sensors.requestTemperatures();
    delayInMillis = 750 / (1 << (12 - resolution));
    lastTempRequest = millis();
  }

  return temperature;
}

// float CoolantTempSensor::getMeasure() {
//   sensors.requestTemperatures();
//   return sensors.getTempCByIndex(0);
// }