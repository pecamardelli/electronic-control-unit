/**
 * @file Gauge.cpp
 * @brief Implementation of the Gauge base class
 *
 * This file implements the functionality for controlling mechanical gauges
 * using stepper motors, including initialization, calibration, and position control.
 * The implementation provides methods for:
 * - Motor initialization and position tracking
 * - Value-to-step conversion using linear interpolation
 * - Calibration and testing procedures
 * - Safe motor control and error handling
 */

#include "Gauge.h"

/**
 * @namespace GaugeConfig
 * @brief Configuration settings for the Gauge class
 *
 * This namespace contains constants that define the behavior of the gauge motor
 * in different operating modes. These values are carefully chosen to provide
 * smooth operation while preventing motor damage.
 */
namespace GaugeConfig
{
    constexpr int INITIAL_MOTOR_SPEED = 5;     ///< Initial motor speed for initialization
    constexpr int CALIBRATION_MOTOR_SPEED = 3; ///< Motor speed for calibration
    constexpr int DEFAULT_MOTOR_SPEED = 1;     ///< Default motor speed for normal operation
}

/**
 * @brief Constructs a new Gauge object
 *
 * Initializes member variables to their default values.
 * The constructor sets up a clean state for the gauge,
 * with both currentStep and stepOffset set to zero.
 */
Gauge::Gauge()
{
    // Initialize member variables
    currentStep = 0;
    stepOffset = 0;
}

/**
 * @brief Destructor for the Gauge object
 *
 * Ensures proper cleanup by stopping the motor if it's still active.
 * This prevents the motor from continuing to run after the gauge is destroyed.
 */
Gauge::~Gauge()
{
    if (motor)
    {
        motor->stop();
    }
}

/**
 * @brief Initializes the gauge
 *
 * Sets up the motor and finds the initial position using the interruptor.
 * The initialization process:
 * 1. Validates motor and interruptor pointers
 * 2. Sets initial motor speed
 * 3. Moves to a known position
 * 4. Returns to the start position
 * 5. Sets the default operating speed
 *
 * @note This method should be called before any other gauge operations
 */
void Gauge::init()
{
    if (!motor || !interruptor)
    {
        return;
    }

    // Set initial motor speed
    motor->setSpeed(GaugeConfig::INITIAL_MOTOR_SPEED);
    interruptor->check();

    // Step forward until button is released
    while (interruptor->isPressed())
    {
        motor->step(1);
        interruptor->check();
    }

    // Get back to the initial position
    goToStartPosition();
    motor->setSpeed(GaugeConfig::DEFAULT_MOTOR_SPEED);
}

/**
 * @brief Moves the gauge to its start position
 *
 * Uses the interruptor to find the zero position and applies any
 * configured offset. The process:
 * 1. Validates motor and interruptor pointers
 * 2. Moves backward until the interruptor is pressed
 * 3. Applies the configured offset
 * 4. Stops the motor
 * 5. Resets the current step counter
 *
 * @note This method assumes the interruptor is properly positioned
 * at the mechanical zero point of the gauge
 */
void Gauge::goToStartPosition()
{
    if (!motor || !interruptor)
    {
        return;
    }

    interruptor->check();

    // Move backward until the interruptor is pressed
    while (!interruptor->isPressed())
    {
        motor->step(-1);
        interruptor->check();
    }

    // Add offset to place the gauge needle correctly
    motor->step(stepOffset);
    motor->stop();

    currentStep = 0;
}

/**
 * @brief Converts a value to the corresponding motor step
 *
 * Uses linear interpolation between conversion points to calculate
 * the exact motor step position for a given value. The process:
 * 1. Validates the conversion table
 * 2. Finds the appropriate conversion points
 * 3. Performs linear interpolation
 * 4. Returns the calculated step position
 *
 * @param value The value to convert (e.g., speed, temperature)
 * @return int The corresponding motor step position
 * @note Returns 0 if the conversion table is empty or if the value
 * is less than the first conversion point
 */
int Gauge::convertToStep(double value)
{
    if (conversions.empty())
    {
        return 0;
    }

    // Find the first conversion that is greater than or equal to the value
    auto conversion = std::find_if(conversions.begin(), conversions.end(),
                                   [value](const Conversion &c)
                                   { return value <= c.value; });

    // If no conversion found or value is less than first conversion
    if (conversion == conversions.begin())
    {
        return 0;
    }

    // Get the lower and upper conversion points
    auto lowerConversion = *(conversion - 1);
    auto upperConversion = *conversion;

    // Calculate the step position using linear interpolation
    float valueDiff = upperConversion.value - lowerConversion.value;
    if (valueDiff == 0)
    {
        return lowerConversion.step;
    }

    uint16_t stepsDiff = upperConversion.step - lowerConversion.step;
    uint16_t stepToGo = lowerConversion.step +
                        static_cast<uint16_t>((value - lowerConversion.value) / valueDiff * stepsDiff);

    return stepToGo;
}

/**
 * @brief Tests the gauge movement
 *
 * Moves the gauge through all positions in the conversion table
 * to verify proper operation. The test:
 * 1. Validates motor and conversion table
 * 2. Moves through each position in sequence
 * 3. Waits for the specified interval
 * 4. Returns to the start position
 *
 * @param motorSpeed The speed to use for the test
 * @param testInterval The delay between positions in seconds
 * @note This method is useful for calibration and debugging
 */
void Gauge::test(long motorSpeed, unsigned int testInterval)
{
    if (!motor || conversions.empty())
    {
        return;
    }

    int previousStep = 0;
    motor->setSpeed(motorSpeed);

    // Move through all positions in the conversion table
    for (const auto &conversion : conversions)
    {
        motor->step(conversion.step - previousStep);
        previousStep = conversion.step;
        sleep_ms(testInterval * 1000);
    }

    // Return to start position
    goToStartPosition();
}

/**
 * @brief Enters calibration mode for the gauge
 *
 * Allows manual adjustment of the gauge position for calibration purposes.
 * The calibration process:
 * 1. Validates motor pointer
 * 2. Enters interactive mode
 * 3. Processes user input for position adjustment
 * 4. Handles return to start position
 * 5. Provides clean exit from calibration mode
 *
 * @note This method provides an interactive interface for fine-tuning
 * the gauge's mechanical position and step offset
 */
void Gauge::calibrate()
{
    if (!motor)
    {
        return;
    }

    int number = 0;
    std::cout << "Entering calibration mode." << std::endl;
    std::cout << "Enter 0 to go to the start position and a non-integer value to exit." << std::endl;

    motor->setSpeed(GaugeConfig::CALIBRATION_MOTOR_SPEED);

    while (true)
    {
        std::cout << "Enter a number to move the gauge needle (current step: " << currentStep << "): ";

        if (!(std::cin >> number))
        {
            std::cout << "Exiting calibration mode." << std::endl;
            currentStep = 0;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }

        if (number == 0)
        {
            goToStartPosition();
        }
        else
        {
            motor->step(number);
            currentStep += number;
        }
    }

    goToStartPosition();
}