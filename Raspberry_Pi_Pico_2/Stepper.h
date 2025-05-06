/*
 * Stepper.h - Stepper library for Raspberry Pi Pico
 */

#ifndef STEPPER_h
#define STEPPER_h

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <cstdlib> // For abs()

class Stepper {
  public:
    // Constructor - specify number of steps per revolution and pins
    Stepper(int number_of_steps, int pin_1, int pin_2);
    Stepper(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4);
    Stepper(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4, int pin_5);
    ~Stepper();

    // Speed setting method:
    void setSpeed(long whatSpeed);
    
    // Move motor specified number of steps
    void step(int number_of_steps);
    
    // Stop the motor
    void stop();
    
    // Library version
    int version(void);

  private:
    void stepMotor(int this_step);
    
    int direction;             // Direction of rotation
    unsigned long step_delay;  // Delay between steps, in us, based on speed
    int number_of_steps;       // Total number of steps this motor can take
    int pin_count;             // Number of motor pins
    int step_number;           // Current step counter
    
    // Motor pin numbers:
    int motor_pin_1;
    int motor_pin_2;
    int motor_pin_3;
    int motor_pin_4;
    int motor_pin_5;
    
    unsigned long last_step_time; // Timestamp in us of when the last step was taken
};

#endif