#include "Relay.h"

Relay::Relay(/* args */)
{
    // Initialize the bcm2835 library
    if (!bcm2835_init())
    {
        logger.error("bcm2835 initialization failed!");
    }

    // Set the relay pin as an output
    bcm2835_gpio_fsel(RELAY_PIN, BCM2835_GPIO_FSEL_OUTP);
}

Relay::~Relay()
{
    // Close the bcm2835 library
    bcm2835_close();
}

void Relay::state(int state)
{
    if (state != LOW && state != HIGH)
    {
        logger.error("Invalid Relay state: " + state);
    }

    bcm2835_gpio_write(RELAY_PIN, state); // HIGH for some modules, LOW for others
}