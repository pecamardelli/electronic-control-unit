#include "VoltSensor.h"

VoltSensor::VoltSensor(/* args */)
{
    description = "VoltSensor";
    logger = std::make_unique<Logger>(description);
}

VoltSensor::~VoltSensor()
{
}

float VoltSensor::getValue()
{
    logger->debug("Calculating voltage from raw value.");

    int rawValue = ads1115.getRawValue(VOLT_SENSOR_CHANNEL);
    float volts = rawValue * 4.096 / 32768.0 * 5; // Scaling factor for MH 25V sensor

    logger->debug("Voltage calculated: " + std::to_string(volts));

    return volts > 0 ? volts : 0.0;
}