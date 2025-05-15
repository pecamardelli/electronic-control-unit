/**
 * @file Stepper.h
 * @brief Header file for the Stepper motor control library
 *
 * This library provides functionality for controlling stepper motors with 2, 4, or 5 wires
 * on the Raspberry Pi Pico. It supports variable speed control and precise positioning.
 */

#ifndef STEPPER_h
#define STEPPER_h

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <cstdlib> // For abs()

/**
 * @class Stepper
 * @brief Controls stepper motors with variable speed and precise positioning
 *
 * The Stepper class provides functionality for controlling stepper motors with
 * different wire configurations (2, 4, or 5 wires). It handles motor initialization,
 * speed control, and step-by-step movement.
 */
class Stepper
{
public:
  /**
   * @brief Constructor for 2-wire stepper motor configuration
   *
   * @param number_of_steps Number of steps per revolution
   * @param pin_1 First motor control pin
   * @param pin_2 Second motor control pin
   */
  Stepper(int number_of_steps, int pin_1, int pin_2);
  Stepper(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4);
  Stepper(int number_of_steps, int pin_1, int pin_2, int pin_3, int pin_4, int pin_5);
  /**
   * @brief Destructor for the Stepper object
   *
   * Ensures proper cleanup by stopping the motor.
   */
  ~Stepper();

  /**
   * @brief Sets the motor speed in revolutions per minute
   *
   * @param whatSpeed Speed in RPM
   */
  void setSpeed(long whatSpeed);

  /**
   * @brief Moves the motor a specified number of steps
   *
   * @param number_of_steps Number of steps to move (negative for reverse)
   */
  void step(int number_of_steps);

  /**
   * @brief Stops the motor by setting all pins to LOW
   */
  void stop();

  /**
   * @brief Returns the library version
   *
   * @return int Library version number
   */
  int version() const;

private:
  /**
   * @brief Controls the motor pins for a single step
   *
   * @param this_step Current step number (0-3 for 2/4 wire, 0-9 for 5 wire)
   */
  void stepMotor(int this_step);

  int direction;            ///< Direction of rotation (0 = backward, 1 = forward)
  unsigned long step_delay; ///< Delay between steps in microseconds
  int number_of_steps;      ///< Total number of steps per revolution
  int pin_count;            ///< Number of motor control pins (2, 4, or 5)
  int step_number;          ///< Current step position (0 to number_of_steps-1)

  // Motor pin numbers
  int motor_pin_1; ///< First motor control pin
  int motor_pin_2; ///< Second motor control pin
  int motor_pin_3; ///< Third motor control pin
  int motor_pin_4; ///< Fourth motor control pin
  int motor_pin_5; ///< Fifth motor control pin

  unsigned long last_step_time; ///< Timestamp of last step in microseconds
};

#endif