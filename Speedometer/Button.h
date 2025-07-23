#pragma once

#include "pico/stdlib.h"

class Button {
private:
    uint pin;
    bool pressed;
    bool pullup;
    
public:
    Button(uint _pin, bool _pullup = true);
    ~Button();
    
    void check();
    bool isPressed() { return pressed; }
};