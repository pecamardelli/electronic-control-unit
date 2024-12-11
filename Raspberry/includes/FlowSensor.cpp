#include "FlowSensor.h"

FlowSensor::FlowSensor(/* args */)
{
    // Initialize bcm2835
    if (!bcm2835_init())
    {
        printf("bcm2835_init failed. Are you running as root?\n");
    }

    // Configure GPIO pin
    bcm2835_gpio_fsel(FLOW_SENSOR_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(FLOW_SENSOR_PIN, BCM2835_GPIO_PUD_UP);

    printf("Monitoring flow sensor...\n");

    // Variables for detecting rising edges
    last_state = bcm2835_gpio_lev(FLOW_SENSOR_PIN);
}

FlowSensor::~FlowSensor()
{
}

void FlowSensor::checkPulses()
{
    // Read the current state of the pin
    current_state = bcm2835_gpio_lev(FLOW_SENSOR_PIN);

    // Detect a rising edge
    if (last_state == LOW && current_state == HIGH)
    {
        pulse_count++;
    }

    // Update the last state
    last_state = current_state;
}