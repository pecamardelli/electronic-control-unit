#include "SpeedSensor.h"

SpeedSensor::SpeedSensor(/* args */)
{
    description = "SpeedSensor";
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);

    loopInterval = config->get<useconds_t>("loop_interval");
    gearRatio = config->get<double>("differential_crown") / config->get<double>("differential_pinion");
    tireWidth = config->get<double>("tire_width");
    aspectRatio = config->get<double>("aspect_ratio");
    rimDiameter = config->get<double>("rim_diameter");
    transitionsPerDriveshaftRev = config->get<double>("transitions_per_lap");
    demultiplication = config->get<double>("demultiplication");
    carStoppedInterval = config->get<double>("car_stopped_interval");
    tireCircumference = calculateTireCircumference();
    kilometersPerTransition = 1.0 * demultiplication / 4.0 / gearRatio * tireCircumference / 1000.0;

    testEnabled = config->get<bool>("test_enabled");
    testInterval = config->get<uint64_t>("test_interval");
    speedModifier = config->get<double>("speed_modifier");

    lowerSpeed = config->get<double>("lower_speed");
    higherSpeed = config->get<double>("higher_speed");

    if (lowerSpeed < 0)
        lowerSpeed = 0;
    if (higherSpeed > 240)
        higherSpeed = 240;
    if (lowerSpeed > higherSpeed)
    {
        lowerSpeed = 0;
        higherSpeed = 240;
    }

    // Initialize the shared data
    speedSensorData->transitions = 0;
    speedSensorData->speed = testEnabled ? lowerSpeed : 0.0;
    speedSensorData->distanceCovered = 0.0;
    speedSensorData->averageSpeed = 0.0;

    logger->info("Gear ratio: " + std::to_string(gearRatio) + " - Tire circumference: " + std::to_string(tireCircumference));

    // Configure D0_PIN as input
    bcm2835_gpio_fsel(D0_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(D0_PIN, BCM2835_GPIO_PUD_UP); // Enable pull-up resistor
}

SpeedSensor::~SpeedSensor()
{
}

void SpeedSensor::loop()
{
    while (!terminateFlag.load())
    {
        // Read the current time in microseconds
        currentTime = bcm2835_st_read();

        // Simulation mode for testing
        if (testEnabled)
        {
            if (speedSensorData->speed < lowerSpeed || speedSensorData->speed > higherSpeed)
                speedModifier *= -1;

            speedSensorData->speed = speedSensorData->speed + speedModifier;
            speedSensorData->distanceCovered = speedSensorData->distanceCovered + 0.1;
        }
        else
        {
            // Real sensor mode
            // Read the current state of the digital output (D0)
            currentState = bcm2835_gpio_lev(D0_PIN);

            // Detect a transition from HIGH to LOW (object detection edge)
            if (lastState == HIGH && currentState == LOW)
            {
                speedSensorData->transitions = speedSensorData->transitions + 1;

                if (lastTime != 0)
                {
                    // Calculate speed based on the time difference since the last transition
                    lastTransitionDuration = currentTime - lastTime;
                    speedSensorData->speed = calculateSpeed(lastTransitionDuration);
                }

                speedSensorData->distanceCovered = kilometersPerTransition * speedSensorData->transitions;

                // Update the last detection time
                lastTime = currentTime;
            }

            // If there's been at least one transition, estimate speed
            if (lastTime != 0)
            {
                // Calculate the time elapsed since the last transition
                elapsedTimeSinceLastTransition = currentTime - lastTime;

                // If no transition has occurred for a while, gradually reduce the speed to zero
                if (elapsedTimeSinceLastTransition > lastTransitionDuration)
                {
                    speedSensorData->speed = calculateSpeed(elapsedTimeSinceLastTransition);
                }

                // If the car is considered stopped, set the speed to 0
                if (elapsedTimeSinceLastTransition > carStoppedInterval * 1e6)
                {
                    speedSensorData->speed = 0;
                }
            }
            else
            {
                // No transitions detected yet, so set speed to 0
                speedSensorData->speed = 0;
            }

            // Update the last state
            lastState = currentState;
        }

        // Sleep to control loop execution frequency
        std::this_thread::sleep_for(std::chrono::microseconds(testEnabled ? testInterval : loopInterval));
    }
}

double SpeedSensor::calculateTireCircumference()
{
    // Tire circumference in meters
    return M_PI * (tireWidth * aspectRatio / 100.0 * 2 + rimDiameter * 25.4) / 1000.0;
}

double SpeedSensor::calculateSpeed(uint64_t elapsedTime)
{
    if (elapsedTime == 0)
    {
        return 0.0; // Avoid division by zero
    }

    driveshaftRevsPerSecond = 1e6 / elapsedTime / transitionsPerDriveshaftRev * demultiplication;

    // Wheel laps per second
    wheelRevsPerSecond = driveshaftRevsPerSecond / gearRatio;

    // Speed in meters per second
    metersPerSecond = wheelRevsPerSecond * tireCircumference;

    // Convert to kilometers per hour
    return metersPerSecond * 3.6;
}