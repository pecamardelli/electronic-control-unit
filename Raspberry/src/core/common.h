#pragma once

#include <string>

#define I2C_BUS "/dev/i2c-1"
#define SPI1_DEVICE "/dev/spidev1.0"

#define ASSETS_PATH "./src/assets"
#define IMAGES_PATH ASSETS_PATH "/images"
#define HOLLEY_SNIPER_PATH ASSETS_PATH "/HolleySniper"
#define HOLLEY_SNIPER_DBC_FILE HOLLEY_SNIPER_PATH "/Sniper_V2.json"

#ifndef CHILD_PROCESS_H_
#define CHILD_PROCESS_H_
typedef struct _childProcess
{
    pid_t pid;
    std::string description;
} ChildProcess;
#endif

#ifndef ENGINE_VALUES_H_
#define ENGINE_VALUES_H_
typedef struct alignas(64) _engineValues
{
    double kml;
    double volts;
    bool ignition;
} EngineValues;
#endif

#ifndef SPEED_SENSOR_DATA_H_
#define SPEED_SENSOR_DATA_H_
typedef struct alignas(64) _speedSensorData
{
    uint64_t transitions;
    double speed;
    double distanceCovered;
    double averageSpeed;
} SpeedSensorData;
#endif

#ifndef COOLANT_TEMP_SENSOR_DATA_H_
#define COOLANT_TEMP_SENSOR_DATA_H_
typedef struct alignas(64) _coolantTempSensorData
{
    float temp;
} CoolantTempSensorData;
#endif

#ifndef FUEL_CONSUMPTION_DATA_H_
#define FUEL_CONSUMPTION_DATA_H_
typedef struct alignas(64) _fuelConsumptionData
{
    double fuelConsumption;
} FuelConsumptionData;
#endif

#ifndef MILEAGE_DATA_H_
#define MILEAGE_DATA_H_
typedef struct alignas(64) _mileageData
{
    uint64_t total = 0;
    uint64_t lastTotal = 0;
    uint64_t lastTotalSaved = 0;
    uint64_t currentTotal = 0;
    float partial = 0;
    float lastPartial = 0;
    float lastPartialSaved = 0;
    float currentPartial = 0;
} MileageData;
#endif

enum Screen
{
    TORINO_LOGO,
    DIGITAL_GAUGE
};