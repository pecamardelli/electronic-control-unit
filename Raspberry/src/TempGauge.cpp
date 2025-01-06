#include "TempGauge.h"

TempGauge::TempGauge(/* args */)
{
    description = "TempGauge";
    motor = std::make_unique<Stepper>(2038, IN1, IN3, IN2, IN4);
    interruptor = std::make_unique<Button>(INTERRUPTOR);
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);

    logger->info("Setting up...");

    loopInterval = config->get<useconds_t>("loop_interval");
    stepOffset = config->get<int>("step_offset");

    conversions.emplace_back(20, config->get<int>("temp_20_step"));
    conversions.emplace_back(40, config->get<int>("temp_40_step"));
    conversions.emplace_back(60, config->get<int>("temp_60_step"));
    conversions.emplace_back(80, config->get<int>("temp_80_step"));
    conversions.emplace_back(100, config->get<int>("temp_100_step"));
    conversions.emplace_back(130, config->get<int>("temp_130_step"));

    init();

    if (config->get<bool>("test_enabled"))
    {
        test(config->get<unsigned int>("test_motor_speed"), config->get<unsigned int>("test_wait_time"));
    }

    if (config->get<bool>("calibration_enabled"))
    {
        calibrate();
    }

    motor->setSpeed(1);
    logger->info("Ready!");
}

TempGauge::~TempGauge()
{
}

void TempGauge::loop(EngineValues *engineValues)
{
    int stepToGo;
    float temp = 0;

    while (!terminateFlag.load())
    {
        temp = engineValues->temp.load();

        if (!temp || temp < 0)
            temp = 0;
        else if (temp > 130)
            temp = 130;

        stepToGo = convertToStep(temp);

        if (currentStep < stepToGo)
        {
            motor->step(1);
            currentStep++;
        }
        else if (currentStep > stepToGo)
        {
            motor->step(-1);
            currentStep--;
        }
        else
        {
            motor->stop();
        }

        usleep(loopInterval);
    }

    logger->info("Terminating process...");
    motor->step(-currentStep);
    logger->info("Done!");
}