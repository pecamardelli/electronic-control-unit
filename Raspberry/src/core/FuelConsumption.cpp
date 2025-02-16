#include "FuelConsumption.h"

FuelConsumption::FuelConsumption(/* args */) : mainSensor(MAIN_SENSOR_PIN), returnSensor(RETURN_SENSOR_PIN)
{
    description = "FuelConsumption";
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);

    mainSensorPulsesPerLiter = mainSensor.getPulsesPerLiter();
    returnSensorPulsesPerLiter = returnSensor.getPulsesPerLiter();

    loopInterval = config->get<useconds_t>("loop_interval");
    testEnabled = config->get<bool>("test_enabled");

    logger->info("Fuel Consumption Process started");
}

FuelConsumption::~FuelConsumption()
{
}

void FuelConsumption::loop()
{
    while (!terminateFlag.load())
    {
        mainPulseCounter = mainSensor.getPulseCount();
        returnPulseCounter = returnSensor.getPulseCount();

        fuelConsumptionData->fuelConsumption = (mainPulseCounter / mainSensorPulsesPerLiter) - (returnPulseCounter / returnSensorPulsesPerLiter);

        // Sleep to control loop execution frequency
        std::this_thread::sleep_for(std::chrono::microseconds(loopInterval));
    }
}