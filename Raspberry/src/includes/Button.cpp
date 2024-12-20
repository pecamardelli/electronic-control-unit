#include "Button.h"

Button::Button(RPiGPIOPin pin)
{
    if (!bcm2835_init())
    {
        std::cerr << "Failed to initialize bcm2835!" << std::endl;
    }

    // Set BUTTON_PIN as input with a pull-up resistor
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP);
}

Button::~Button()
{
}

void Button::check()
{
    uint8_t buttonState = bcm2835_gpio_lev(pin);

    if (buttonState == LOW)
    {
        // std::cout << "Button is PRESSED." << std::endl;
        pressed = true;
    }
    else
    {
        // std::cout << "Button is RELEASED." << std::endl;
        pressed = false;
    }
}