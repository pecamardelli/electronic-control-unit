#include "TempGauge.h"

TempGauge::TempGauge(/* args */)
{
    description = "TempGauge";
    logger = new Logger(description);
    config = new Config(description);

    loopInterval = config->get<useconds_t>("loop_interval");
    stepOffset = config->get<int>("step_offset");

    conversions[0].step = config->get<int>("temp_20_step");
    conversions[1].step = config->get<int>("temp_40_step");
    conversions[2].step = config->get<int>("temp_60_step");
    conversions[3].step = config->get<int>("temp_80_step");
    conversions[4].step = config->get<int>("temp_100_step");
    conversions[5].step = config->get<int>("temp_130_step");

    motor.setSpeed(3);

    logger->info("Setting up...");
    button.check();

    // Step forward until button is released.
    while (button.pressed)
    {
        motor.step(1);
        button.check();
    }

    // Get back to the initial position.
    goToStartPosition();

    if (config->get<bool>("test_enabled"))
    {
        test();
    }

    motor.setSpeed(1);

    logger->info("Temp Gauge ready!");
}

TempGauge::~TempGauge()
{
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
    motor.step(-currentStep);
    logger->info("Done!");

    delete logger;
}

void TempGauge::goToStartPosition()
{
    logger->info("Going to start position...");
    button.check();

    while (!button.pressed)
    {
        motor.step(-1);
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
    motor.setSpeed(config->get<long>("test_motor_speed"));

    for (const auto &conversion : conversions)
    {
        logger->info("Placing needle at " + std::to_string(conversion.temp) + " degrees. Steps: " + std::to_string(conversion.step));
        motor.step(conversion.step - previousStep);
        previousStep = conversion.step;
        sleep(config->get<unsigned int>("test_wait_time"));
    }

    goToStartPosition();

    logger->info("Done...");
}