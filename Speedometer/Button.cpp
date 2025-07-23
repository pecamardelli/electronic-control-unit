// button.cpp
#include "Button.h"

Button::Button(uint _pin, bool _pullup)
{
    pin = _pin;
    pullup = _pullup;
    pressed = false;

    // Initialize the GPIO pin as an input
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    
    // Set pull-up resistor if requested
    if (pullup) {
        gpio_pull_up(pin);
    } else {
        gpio_pull_down(pin);
    }
}

Button::~Button()
{
    // Nothing special needed for cleanup
}

void Button::check()
{
    // Read the pin level
    // If using pull-up: LOW means pressed
    // If using pull-down: HIGH means pressed
    if (pullup) {
        pressed = (gpio_get(pin) == 0);
    } else {
        pressed = (gpio_get(pin) == 1);
    }
}