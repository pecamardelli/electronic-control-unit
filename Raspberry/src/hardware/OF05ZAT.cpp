#include "OF05ZAT.h"

OF05ZAT::OF05ZAT(RPiGPIOPin _pin) : pin(_pin)
{
    description = "OF05ZAT";
    config = std::make_unique<Config>(description);

    pulsesPerLiter = config->get<uint16_t>("pulses_per_liter");
    pulseCounter = 0;

    // Configure GPIO pin
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP);

    // Variables for detecting rising edges
    lastState = bcm2835_gpio_lev(pin);
}

OF05ZAT::~OF05ZAT()
{
}

void OF05ZAT::checkPulse()
{
    // Read the current state of the pin
    currentState = bcm2835_gpio_lev(pin);

    // Detect a rising edge
    if (lastState == LOW && currentState == HIGH)
    {
        pulseCounter++;
    }

    // Update the last state
    lastState = currentState;
}

uint64_t OF05ZAT::getPulseCount()
{
    return pulseCounter;
}

uint16_t OF05ZAT::getPulsesPerLiter()
{
    return pulsesPerLiter;
}