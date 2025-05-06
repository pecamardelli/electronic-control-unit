/*
 * Stepper.cpp - Stepper library for Raspberry Pi Pico - Adapted from original Arduino/RPi library
 *
 * Original library        (0.1)   by Tom Igoe.
 * Two-wire modifications  (0.2)   by Sebastian Gassner
 * Combination version     (0.3)   by Tom Igoe and David Mellis
 * Bug fix for four-wire   (0.4)   by Tom Igoe, bug fix from Noah Shibley
 * High-speed stepping mod         by Eugene Kozlenko
 * Timer rollover fix              by Eugene Kozlenko
 * Five phase five wire    (1.1.0) by Ryan Orendorff
 * Ported to Raspberry PI  (1.1.0) by Pablo Camardelli
 * Adapted for Pico                by [Your Name]
 */

#include "Stepper.h"

/*
 * Two-wire constructor.
 * Sets which wires should control the motor.
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

/*
 * Four-pin constructor.
 * Sets which wires should control the motor.
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

/*
 * Five-pin constructor.
 * Sets which wires should control the motor.
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

Stepper::~Stepper()
{
  stop();
}

/*
 * Sets the speed in revs per minute
 */
void Stepper::setSpeed(long whatSpeed)
{
  step_delay = 60L * 1000L * 1000L / number_of_steps / whatSpeed;
}

/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
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

/*
 * Moves the motor forward or backwards.
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

void Stepper::stop()
{
  gpio_put(motor_pin_1, 0);
  gpio_put(motor_pin_2, 0);
  if (pin_count >= 4) {
    gpio_put(motor_pin_3, 0);
    gpio_put(motor_pin_4, 0);
    if (pin_count == 5) {
      gpio_put(motor_pin_5, 0);
    }
  }
}

/*
  version() returns the version of the library:
*/
int Stepper::version(void) { return 6; }