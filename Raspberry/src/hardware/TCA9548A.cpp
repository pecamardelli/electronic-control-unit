#include "TCA9548A.h"

TCA9548A::TCA9548A(/* args */)
{
    logger = std::make_unique<Logger>(description);
}

TCA9548A::~TCA9548A()
{
}

// Function to select a channel on the TCA9548A
void TCA9548A::selectChannel(uint8_t channel)
{
    if (channel > 7)
    {
        logger->error("Invalid channel number. Must be between 0 and 7.");
        return;
    }

    if (channel == currentChannel)
        return;

    bcm2835_i2c_setSlaveAddress(TCA9548A_ADDRESS);

    char data = 1 << channel; // Bitmask for the selected channel
    if (bcm2835_i2c_write(&data, 1))
    {
        logger->error("Failed to select channel.");
    }
    else
    {
        currentChannel = channel;
    }
}

uint8_t TCA9548A::getCurrentChannel()
{
    return currentChannel;
}

void TCA9548A::reset()
{
    // std::cout << "Resetting TCA9548A..." << std::endl;

    // bcm2835_gpio_fsel(RESET_PIN, BCM2835_GPIO_FSEL_OUTP); // Set as output

    // bcm2835_gpio_write(RESET_PIN, LOW);
    // bcm2835_delay(10); // Hold reset low for 10ms
    // bcm2835_gpio_write(RESET_PIN, HIGH);
    // bcm2835_delay(10); // Wait for the chip to recover

    // std::cout << "TCA9548A reset complete." << std::endl;
}