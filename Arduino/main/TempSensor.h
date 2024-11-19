/**************************************************************************/
/*!
  @file     TempSensor.h

  Original library by Pablin defining the logic for the DHT sensor.

  License: MIT (see LICENSE)
*/
/**************************************************************************/

#ifndef _TEMPSENSOR_H
#define _TEMPSENSOR_H

// Incluimos librería
#include <DHT.h>
#include <RTClib.h>

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

typedef struct _readings
{
  float temp;
  float humidity;
} readings;

class TempSensor
{
private:
  /* data */
  DateTime now;

public:
  TempSensor(/* args */);
  ~TempSensor();

  void begin();
  readings getReadings();
};

#endif // _TEMPSENSOR_H
