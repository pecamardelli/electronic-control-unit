#include "FlowSensor.h"

FlowSensor::FlowSensor(/* args */)
{
    className = "FlowSensor";
    // Initialize bcm2835
    if (!bcm2835_init())
    {
        error("bcm2835_init failed. Are you running as root?");
    }

    // Configure GPIO pin
    bcm2835_gpio_fsel(FLOW_SENSOR_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(FLOW_SENSOR_PIN, BCM2835_GPIO_PUD_UP);

    info("Flow sensor inited...");

    // Variables for detecting rising edges
    lastState = bcm2835_gpio_lev(FLOW_SENSOR_PIN);
}

FlowSensor::~FlowSensor()
{
}

FlowSensorData FlowSensor::loop()
{
    // Read the current state of the pin
    currentState = bcm2835_gpio_lev(FLOW_SENSOR_PIN);

    // Detect a rising edge
    if (lastState == LOW && currentState == HIGH)
    {
        data.partialPulseCount++;
        data.totalPulseCount++;
        data.totalConsumption = (float)data.totalPulseCount / PULSES_PER_LITER;
    }

    // Update the last state
    lastState = currentState;

    return data;
}