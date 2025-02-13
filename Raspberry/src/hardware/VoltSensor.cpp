#include "VoltSensor.h"

VoltSensor::VoltSensor(ADS1115 *_ads1115)
{
    description = "VoltSensor";
    logger = std::make_unique<Logger>(description);
    ads1115 = _ads1115;
}

VoltSensor::~VoltSensor()
{
}

float VoltSensor::getValue()
{
    logger->debug("Calculating voltage from raw value.");

    int rawValue = ads1115->getRawValue(VOLT_SENSOR_CHANNEL);
    float volts = rawValue * 4.096 / 32768.0 * 5; // Scaling factor for MH 25V sensor

    logger->debug("Voltage calculated: " + std::to_string(volts));

    return volts > 0 ? volts : 0.0;
}