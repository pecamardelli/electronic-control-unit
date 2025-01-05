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
    tireCircumference = calculateTireCircumference();
    kilometersPerTransition = 1.0 / gearRatio / 4.0 * tireCircumference / 1000.0;

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
    uint32_t transitions = 0;
    uint8_t lastState = LOW; // Store the last state of the sensor
    uint64_t lastTime = 0;   // Time of the last detection in microseconds
    uint8_t currentState;
    uint64_t currentTime;
    uint64_t elapsedTime;

    while (!terminateFlag.load())
    {
        // Read the current state of the digital output (D0)
        currentState = bcm2835_gpio_lev(D0_PIN);

        // Detect a transition from HIGH to LOW (object detection edge)
        if (lastState == HIGH && currentState == LOW)
        {
            transitions++;
            engineValues->distanceCovered.store(engineValues->distanceCovered.load() + kilometersPerTransition);

            // Get the current time in microseconds
            currentTime = bcm2835_st_read();

            if (lastTime != 0)
            {
                // If this isn't the first detection
                engineValues->speed = calculateSpeed(currentTime - lastTime);
                std::cout << "Object detected! Count: " << transitions
                          << ", Speed: " << engineValues->speed << " km/h, distance: " << engineValues->distanceCovered << std::endl;
            }

            // Update the last detection time
            lastTime = currentTime;
        }

        // Update the last state
        lastState = currentState;

        // Small delay to debounce the signal
        bcm2835_delay(loopInterval);
    }
}

double SpeedSensor::calculateTireCircumference()
{
    // Tire circumference in meters
    double tireDiameter = (tireWidth * aspectRatio / 100.0 * 2 + rimDiameter * 25.4) / 1000.0;
    return M_PI * tireDiameter;
}

double SpeedSensor::calculateSpeed(uint64_t elapsedTime)
{
    if (elapsedTime == 0)
    {
        return 0.0; // Avoid division by zero
    }

    // Driveshaft laps per second
    double driveshaftLapsPerSecond = 1e6 / elapsedTime / transitionsPerLap;

    // Wheel laps per second
    double wheelLapsPerSecond = driveshaftLapsPerSecond / gearRatio;

    // Speed in meters per second
    double speedMps = wheelLapsPerSecond * tireCircumference;

    // Convert to kilometers per hour
    return speedMps * 3.6;
}