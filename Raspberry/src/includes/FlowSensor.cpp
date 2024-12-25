#include "FlowSensor.h"

FlowSensor::FlowSensor(/* args */)
{
    description = "FlowSensor";
    logger = new Logger(description);
    config = new Config(description);

    loopInterval = config->get<useconds_t>("loop_interval");
    pulsesPerLiter = config->get<u_int16_t>("pulses_per_liter");

    BCM2835Manager::getInstance();

    // Configure GPIO pin
    bcm2835_gpio_fsel(FLOW_SENSOR_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(FLOW_SENSOR_PIN, BCM2835_GPIO_PUD_UP);

    // Variables for detecting rising edges
    lastState = bcm2835_gpio_lev(FLOW_SENSOR_PIN);

    logger->info("Flow sensor inited...");
}

FlowSensor::~FlowSensor()
{
    delete logger;
}

void FlowSensor::loop(EngineValues *engineValues)
{
    // Read the current state of the pin
    currentState = bcm2835_gpio_lev(FLOW_SENSOR_PIN);

    // Detect a rising edge
    if (lastState == LOW && currentState == HIGH)
    {
        data.partialPulseCount++;
        data.totalPulseCount++;
        data.totalConsumption = (float)data.totalPulseCount / pulsesPerLiter;
    }

    // Update the last state
    lastState = currentState;

    engineValues->fuelConsumption.store(data.totalConsumption);
    engineValues->kml.store(data.totalPulseCount);

    usleep(loopInterval);
}