#include "Button.h"

Button::Button(RPiGPIOPin _pin)
{
    pin = _pin;

    // Set the pin as input with a pull-up resistor
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(pin, BCM2835_GPIO_PUD_UP);
}

Button::~Button()
{
}

void Button::check()
{
    if (bcm2835_gpio_lev(pin) == LOW)
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