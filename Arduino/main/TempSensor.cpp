#include "TempSensor.h"

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

TempSensor::TempSensor(/* args */) {}

TempSensor::~TempSensor() {}

/**************************************************************************/
/*!
    @brief  Initializes the DHT temperature sensor.
*/
/**************************************************************************/
void TempSensor::begin() { dht.begin(); }

/**************************************************************************/
/*!
    @brief  Returns sensor readings.
    @return A struct containing the values of the temperature and humidity.
*/
/**************************************************************************/
readings TempSensor::getReadings() {
  readings values = {dht.readTemperature(), dht.readHumidity()};

  return values;
}