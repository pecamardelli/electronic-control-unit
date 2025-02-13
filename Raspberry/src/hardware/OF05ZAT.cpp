#include "OF05ZAT.h"

OF05ZAT::OF05ZAT(/* args */)
{
    description = "OF05ZAT";
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);

    loopInterval = config->get<useconds_t>("loop_interval");
    pulsesPerLiter = config->get<u_int16_t>("pulses_per_liter");

    // Configure GPIO pin
    bcm2835_gpio_fsel(FLOW_SENSOR_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(FLOW_SENSOR_PIN, BCM2835_GPIO_PUD_UP);

    // Variables for detecting rising edges
    lastState = bcm2835_gpio_lev(FLOW_SENSOR_PIN);

    logger->info("Flow sensor inited...");
}

OF05ZAT::~OF05ZAT()
{
}

void OF05ZAT::loop()
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

    // engineValues->fuelConsumption.store(data.totalConsumption);
    // engineValues->kml.store(data.totalPulseCount);

    std::this_thread::sleep_for(std::chrono::microseconds(loopInterval));
}