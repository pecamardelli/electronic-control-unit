#include "TempGauge.h"

TempGauge::TempGauge(/* args */)
{
    description = "TempGauge";
    logger = new Logger(description);

    loopInterval = sys->getConfigValue<useconds_t>(description, "loop_interval");
    stepOffset = sys->getConfigValue<int>(description, "step_offset");

    conversions[0].step = sys->getConfigValue<int>(description, "temp_20_step");
    conversions[1].step = sys->getConfigValue<int>(description, "temp_40_step");
    conversions[2].step = sys->getConfigValue<int>(description, "temp_60_step");
    conversions[3].step = sys->getConfigValue<int>(description, "temp_80_step");
    conversions[4].step = sys->getConfigValue<int>(description, "temp_100_step");
    conversions[5].step = sys->getConfigValue<int>(description, "temp_130_step");

    motor.setSpeed(2);

    logger->info("Setting up...");
    button.check();

    // Step forward until button is released.
    while (button.pressed)
    {
        motor.step(1);
        usleep(loopInterval);
        button.check();
    }

    // Get back to the initial position.
    goToStartPosition();

    motor.setSpeed(1);

    test();

    logger->info("Temp Gauge ready!");
}

TempGauge::~TempGauge()
{
    logger->info("Going to start position...");
    goToStartPosition();
    logger->info("Done!");

    delete logger;
}

void TempGauge::loop(EngineValues *engineValues)
{
    int stepToGo;
    float temp = 0;

    logger->info("Entering loop...");

    while (!terminateFlag.load())
    {
        temp = engineValues->temp.load();

        if (!temp || temp < 0)
            temp = 0;
        else if (temp > 130)
            temp = 130;

        stepToGo = tempToStep(temp);

        if (currentStep < stepToGo)
        {
            motor.step(1);
            currentStep++;
        }
        else if (currentStep > stepToGo)
        {
            motor.step(-1);
            currentStep--;
        }
        else
        {
            motor.stop();
        }

        usleep(loopInterval);
    }
    logger->info("Terminating process...");
}

void TempGauge::goToStartPosition()
{
    logger->info("Going to start position...");
    button.check();

    while (!button.pressed)
    {
        motor.step(-1);
        usleep(loopInterval);
        button.check();
    }

    // Add offset to place the gauge needle correctly.
    motor.step(stepOffset);
    motor.stop();

    currentStep = 0;
}

uint16_t TempGauge::tempToStep(float temp)
{
    // Find the proper map object.
    auto conversion = std::find_if(conversions.begin(), conversions.end(), [temp](const Conversion &c)
                                   { return temp <= c.temp; });

    // Calculate the index using std::distance
    size_t index = std::distance(conversions.begin(), conversion);

    if (index == 0)
    {
        return 0;
    }

    auto lowerConversion = conversions.at(index - 1);

    float tempDiff = conversion->temp - lowerConversion.temp;
    uint16_t stepsDiff = conversion->step - lowerConversion.step;
    uint16_t stepToGo = lowerConversion.step + (temp - lowerConversion.temp) / tempDiff * stepsDiff;

    return stepToGo;
}

void TempGauge::test()
{
    logger->info("Entering test mode...");
    int previousStep = 0;

    for (const auto &conversion : conversions)
    {
        logger->info("Placing needle at " + std::to_string(conversion.temp) + " degrees. Steps: " + std::to_string(conversion.step));
        motor.step(conversion.step - previousStep);
        previousStep = conversion.step;
        sleep(sys->getConfigValue<unsigned int>(description, "test_wait_time"));
    }

    goToStartPosition();
    logger->info("Done...");
}