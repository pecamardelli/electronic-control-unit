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
    transitionsPerLap = config->get<double>("transitions_per_lap");
    demultiplication = config->get<double>("demultiplication");
    tireCircumference = calculateTireCircumference();
    kilometersPerTransition = 1.0 / gearRatio / 4.0 * tireCircumference / 1000.0;

    speedSensorTransitions = 0;

    logger->info("Gear ratio: " + std::to_string(gearRatio) + " - Tire circumference: " + std::to_string(tireCircumference));

    // Configure D0_PIN as input
    bcm2835_gpio_fsel(D0_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(D0_PIN, BCM2835_GPIO_PUD_UP); // Enable pull-up resistor
}

SpeedSensor::~SpeedSensor()
{
}

void SpeedSensor::loop(EngineValues *engineValues)
{
    while (!terminateFlag.load())
    {
        // Read the current state of the digital output (D0)
        currentState = bcm2835_gpio_lev(D0_PIN);

        // Detect a transition from HIGH to LOW (object detection edge)
        if (lastState == HIGH && currentState == LOW)
        {
            speedSensorTransitions++;
            // engineValues->distanceCovered.store(engineValues->distanceCovered.load() + kilometersPerTransition);

            // Get the current time in microseconds
            currentTime = bcm2835_st_read();

            if (lastTime != 0)
            {
                // If this isn't the first detection
                // engineValues->speed = calculateSpeed(currentTime - lastTime);
                std::cout << "Object detected! Count: " << speedSensorTransitions << std::endl;
            }

            // Update the last detection time
            lastTime = currentTime;
        }

        // Update the last state
        lastState = currentState;

        // Small delay to debounce the signal
        usleep(loopInterval);
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

    driveshaftRevsPerSecond = 1e6 / elapsedTime / transitionsPerLap * demultiplication;

    // Wheel laps per second
    wheelRevsPerSecond = driveshaftRevsPerSecond / gearRatio;

    // Speed in meters per second
    metersPerSecond = wheelRevsPerSecond * tireCircumference;

    // Convert to kilometers per hour
    return metersPerSecond * 3.6;
}