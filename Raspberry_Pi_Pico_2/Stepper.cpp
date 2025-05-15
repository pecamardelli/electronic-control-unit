/**
 * @file Stepper.cpp
 * @brief Implementation of the Stepper motor control library
 *
 * This file implements the functionality for controlling stepper motors with 2, 4, or 5 wires
 * on the Raspberry Pi Pico. The implementation includes:
 * - Motor initialization for different wire configurations
 * - Speed control and timing management
 * - Step-by-step movement control
 * - Pin state management for different motor types
 *
 * Version history:
 * - 0.1: Original library by Tom Igoe
 * - 0.2: Two-wire modifications by Sebastian Gassner
 * - 0.3: Combination version by Tom Igoe and David Mellis
 * - 0.4: Bug fix for four-wire by Tom Igoe, fix from Noah Shibley
 * - 1.1.0: Five phase five wire by Ryan Orendorff
 * - 1.1.0: Ported to Raspberry PI by Pablo Camardelli
 * - 1.1.0: Adapted for Pico by Pablo Camardelli
 */

#include "Stepper.h"

/**
 * @brief Constructor for 2-wire stepper motor configuration
 *
 * Initializes a stepper motor with 2 control wires. Sets up GPIO pins
 * and initializes motor state variables.
 *
 * @param steps Number of steps per revolution
 * @param pin_1 First motor control pin
 * @param pin_2 Second motor control pin
 */
Stepper::Stepper(int steps, int pin_1, int pin_2)
{
  step_number = 0;         // which step the motor is on
  direction = 0;           // motor direction
  last_step_time = 0;      // time stamp in us of the last step taken
  number_of_steps = steps; // total number of steps for this motor

  // Pins for the motor control connection:
  motor_pin_1 = pin_1;
  motor_pin_2 = pin_2;

  // Setup the pins on the microcontroller:
  gpio_init(motor_pin_1);
  gpio_set_dir(motor_pin_1, GPIO_OUT);

  gpio_init(motor_pin_2);
  gpio_set_dir(motor_pin_2, GPIO_OUT);

  // When there are only 2 pins, set the others to 0:
  motor_pin_3 = 0;
  motor_pin_4 = 0;
  motor_pin_5 = 0;

  // pin_count is used by the stepMotor() method:
  pin_count = 2;
}

/**
 * @brief Constructor for 4-wire stepper motor configuration
 *
 * Initializes a stepper motor with 4 control wires. Sets up GPIO pins
 * and initializes motor state variables.
 *
 * @param steps Number of steps per revolution
 * @param pin_1 First motor control pin
 * @param pin_2 Second motor control pin
 * @param pin_3 Third motor control pin
 * @param pin_4 Fourth motor control pin
 */
Stepper::Stepper(int steps, int pin_1, int pin_2,
                 int pin_3, int pin_4)
{
  step_number = 0;         // which step the motor is on
  direction = 0;           // motor direction
  last_step_time = 0;      // time stamp in us of the last step taken
  number_of_steps = steps; // total number of steps for this motor

  // Pins for the motor control connection:
  motor_pin_1 = pin_1;
  motor_pin_2 = pin_2;
  motor_pin_3 = pin_3;
  motor_pin_4 = pin_4;

  // Setup the pins on the microcontroller:
  gpio_init(motor_pin_1);
  gpio_init(motor_pin_2);
  gpio_init(motor_pin_3);
  gpio_init(motor_pin_4);
  gpio_set_dir(motor_pin_1, GPIO_OUT);
  gpio_set_dir(motor_pin_2, GPIO_OUT);
  gpio_set_dir(motor_pin_3, GPIO_OUT);
  gpio_set_dir(motor_pin_4, GPIO_OUT);

  // When there are 4 pins, set the others to 0:
  motor_pin_5 = 0;

  // pin_count is used by the stepMotor() method:
  pin_count = 4;
}

/**
 * @brief Constructor for 5-wire stepper motor configuration
 *
 * Initializes a stepper motor with 5 control wires. Sets up GPIO pins
 * and initializes motor state variables.
 *
 * @param steps Number of steps per revolution
 * @param pin_1 First motor control pin
 * @param pin_2 Second motor control pin
 * @param pin_3 Third motor control pin
 * @param pin_4 Fourth motor control pin
 * @param pin_5 Fifth motor control pin
 */
Stepper::Stepper(int steps, int pin_1, int pin_2,
                 int pin_3, int pin_4, int pin_5)
{
  step_number = 0;         // which step the motor is on
  direction = 0;           // motor direction
  last_step_time = 0;      // time stamp in us of the last step taken
  number_of_steps = steps; // total number of steps for this motor

  // Pins for the motor control connection:
  motor_pin_1 = pin_1;
  motor_pin_2 = pin_2;
  motor_pin_3 = pin_3;
  motor_pin_4 = pin_4;
  motor_pin_5 = pin_5;

  // Setup the pins on the microcontroller:
  gpio_init(motor_pin_1);
  gpio_init(motor_pin_2);
  gpio_init(motor_pin_3);
  gpio_init(motor_pin_4);
  gpio_init(motor_pin_5);
  gpio_set_dir(motor_pin_1, GPIO_OUT);
  gpio_set_dir(motor_pin_2, GPIO_OUT);
  gpio_set_dir(motor_pin_3, GPIO_OUT);
  gpio_set_dir(motor_pin_4, GPIO_OUT);
  gpio_set_dir(motor_pin_5, GPIO_OUT);

  // pin_count is used by the stepMotor() method:
  pin_count = 5;
}

/**
 * @brief Destructor for the Stepper object
 *
 * Ensures proper cleanup by stopping the motor.
 */
Stepper::~Stepper()
{
  stop();
}

/**
 * @brief Sets the motor speed in revolutions per minute
 *
 * Calculates the delay between steps based on the desired speed
 * and the number of steps per revolution.
 *
 * @param whatSpeed Speed in RPM
 */
void Stepper::setSpeed(long whatSpeed)
{
  step_delay = 60L * 1000L * 1000L / number_of_steps / whatSpeed;
}

/**
 * @brief Moves the motor a specified number of steps
 *
 * Controls the motor movement with proper timing and direction.
 * The motor will move one step at a time, with appropriate delays
 * between steps to maintain the set speed.
 *
 * @param steps_to_move Number of steps to move (negative for reverse)
 */
void Stepper::step(int steps_to_move)
{
  int steps_left = abs(steps_to_move); // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move > 0)
  {
    direction = 1;
  }
  if (steps_to_move < 0)
  {
    direction = 0;
  }

  // decrement the number of steps, moving one step each time:
  while (steps_left > 0)
  {
    unsigned long now = time_us_64(); // Get current time in microseconds
    // move only if the appropriate delay has passed:
    if (now - last_step_time >= step_delay)
    {
      // get the timeStamp of when you stepped:
      last_step_time = now;
      // increment or decrement the step number,
      // depending on direction:
      if (direction == 1)
      {
        step_number++;
        if (step_number == number_of_steps)
        {
          step_number = 0;
        }
      }
      else
      {
        if (step_number == 0)
        {
          step_number = number_of_steps;
        }
        step_number--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, ..., {3 or 10}
      if (pin_count == 5)
        stepMotor(step_number % 10);
      else
        stepMotor(step_number % 4);
    }
  }
}

/**
 * @brief Controls the motor pins for a single step
 *
 * Sets the appropriate pin states for the current step position.
 * Different pin patterns are used for 2-wire, 4-wire, and 5-wire motors.
 *
 * @param this_step Current step number (0-3 for 2/4 wire, 0-9 for 5 wire)
 */
void Stepper::stepMotor(int thisStep)
{
  if (pin_count == 2)
  {
    switch (thisStep)
    {
    case 0: // 01
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 1);
      break;
    case 1: // 11
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 1);
      break;
    case 2: // 10
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 0);
      break;
    case 3: // 00
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 0);
      break;
    }
  }
  if (pin_count == 4)
  {
    switch (thisStep)
    {
    case 0: // 1010
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 0);
      gpio_put(motor_pin_3, 1);
      gpio_put(motor_pin_4, 0);
      break;
    case 1: // 0110
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 1);
      gpio_put(motor_pin_3, 1);
      gpio_put(motor_pin_4, 0);
      break;
    case 2: // 0101
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 1);
      gpio_put(motor_pin_3, 0);
      gpio_put(motor_pin_4, 1);
      break;
    case 3: // 1001
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 0);
      gpio_put(motor_pin_3, 0);
      gpio_put(motor_pin_4, 1);
      break;
    }
  }

  if (pin_count == 5)
  {
    switch (thisStep)
    {
    case 0: // 01101
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 1);
      gpio_put(motor_pin_3, 1);
      gpio_put(motor_pin_4, 0);
      gpio_put(motor_pin_5, 1);
      break;
    case 1: // 01001
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 1);
      gpio_put(motor_pin_3, 0);
      gpio_put(motor_pin_4, 0);
      gpio_put(motor_pin_5, 1);
      break;
    case 2: // 01011
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 1);
      gpio_put(motor_pin_3, 0);
      gpio_put(motor_pin_4, 1);
      gpio_put(motor_pin_5, 1);
      break;
    case 3: // 01010
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 1);
      gpio_put(motor_pin_3, 0);
      gpio_put(motor_pin_4, 1);
      gpio_put(motor_pin_5, 0);
      break;
    case 4: // 11010
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 1);
      gpio_put(motor_pin_3, 0);
      gpio_put(motor_pin_4, 1);
      gpio_put(motor_pin_5, 0);
      break;
    case 5: // 10010
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 0);
      gpio_put(motor_pin_3, 0);
      gpio_put(motor_pin_4, 1);
      gpio_put(motor_pin_5, 0);
      break;
    case 6: // 10110
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 0);
      gpio_put(motor_pin_3, 1);
      gpio_put(motor_pin_4, 1);
      gpio_put(motor_pin_5, 0);
      break;
    case 7: // 10100
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 0);
      gpio_put(motor_pin_3, 1);
      gpio_put(motor_pin_4, 0);
      gpio_put(motor_pin_5, 0);
      break;
    case 8: // 10101
      gpio_put(motor_pin_1, 1);
      gpio_put(motor_pin_2, 0);
      gpio_put(motor_pin_3, 1);
      gpio_put(motor_pin_4, 0);
      gpio_put(motor_pin_5, 1);
      break;
    case 9: // 00101
      gpio_put(motor_pin_1, 0);
      gpio_put(motor_pin_2, 0);
      gpio_put(motor_pin_3, 1);
      gpio_put(motor_pin_4, 0);
      gpio_put(motor_pin_5, 1);
      break;
    }
  }
}

/**
 * @brief Stops the motor by setting all pins to LOW
 *
 * Safely stops the motor by de-energizing all control pins.
 */
void Stepper::stop()
{
  gpio_put(motor_pin_1, 0);
  gpio_put(motor_pin_2, 0);
  if (pin_count >= 4)
  {
    gpio_put(motor_pin_3, 0);
    gpio_put(motor_pin_4, 0);
    if (pin_count == 5)
    {
      gpio_put(motor_pin_5, 0);
    }
  }
}

/**
 * @brief Returns the library version
 *
 * @return int Library version number (110 for version 1.1.0)
 */
int Stepper::version() const { return 110; }