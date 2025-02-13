#include "RELAY.h"

RELAY::RELAY(/* args */)
{
    // Set the relay pin as an output
    bcm2835_gpio_fsel(RELAY_PIN, BCM2835_GPIO_FSEL_OUTP);
}

RELAY::~RELAY()
{
}

void RELAY::state(int state)
{
    if (state != LOW && state != HIGH)
    {
        logger.error("Invalid RELAY state: " + state);
    }

    bcm2835_gpio_write(RELAY_PIN, state); // HIGH for some modules, LOW for others
}