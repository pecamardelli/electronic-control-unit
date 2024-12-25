#include "SpeedSensor.h"

SpeedSensor::SpeedSensor(/* args */)
{
    description = "SpeedSensor";
    loopInterval = sys->getConfigValue<useconds_t>(description, "loop_interval");
    // Configure D0_PIN as input
    bcm2835_gpio_fsel(D0_PIN, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(D0_PIN, BCM2835_GPIO_PUD_UP); // Enable pull-up resistor
}

SpeedSensor::~SpeedSensor()
{
}

void SpeedSensor::loop(EngineValues *engineValues)
{
    uint8_t current_state;

    while (!terminateFlag.load())
    {
        // Read the current state of the digital output (D0)
        current_state = bcm2835_gpio_lev(D0_PIN);
        uint64_t current_time;
        uint64_t elapsed_time;

        // Detect a transition from HIGH to LOW (object detection edge)
        if (last_state == HIGH && current_state == LOW)
        {
            object_count++;

            // Get the current time in microseconds
            current_time = bcm2835_st_read();

            if (last_time != 0)
            {                                            // If this isn't the first detection
                elapsed_time = current_time - last_time; // Calculate time elapsed
                std::cout << "Object detected! Count: " << object_count
                          << ", Time elapsed: " << elapsed_time << " µs" << std::endl;
            }
            else
            {
                std::cout << "Object detected! Count: " << object_count
                          << ", First detection (no elapsed time)." << std::endl;
            }

            // Update the last detection time
            last_time = current_time;
        }

        // Update the last state
        last_state = current_state;

        // Small delay to debounce the signal
        bcm2835_delay(loopInterval);
    }
}