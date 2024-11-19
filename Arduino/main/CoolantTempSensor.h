#ifndef _WATER_TEMP_SENSOR_H_
#define _WATER_TEMP_SENSOR_H_

#include <DallasTemperature.h>
#include <OneWire.h>

class CoolantTempSensor {
 private:
  DeviceAddress tempDeviceAddress;
  bool inited = false;
  int resolution = 12;
  unsigned long lastTempRequest = 0;
  int delayInMillis = 0;
  float temperature = 0.0;
  void setup();
  /* data */
 public:
  CoolantTempSensor(/* args */);
  ~CoolantTempSensor();
  float getMeasure();
};

#endif