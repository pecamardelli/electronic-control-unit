#include "Speedometer.h"

Speedometer::Speedometer(/* args */)
{
    description = "Speedometer";

    motor = std::make_unique<STEPPER>(2038, IN1, IN3, IN2, IN4);
    interruptor = std::make_unique<Button>(INTERRUPTOR);
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);

    logger->info("Setting up...");

    conversions.emplace_back(0, 0);
    conversions.emplace_back(20, config->get<int>("km_20_step"));
    conversions.emplace_back(30, config->get<int>("km_30_step"));
    conversions.emplace_back(40, config->get<int>("km_40_step"));
    conversions.emplace_back(50, config->get<int>("km_50_step"));
    conversions.emplace_back(60, config->get<int>("km_60_step"));
    conversions.emplace_back(70, config->get<int>("km_70_step"));
    conversions.emplace_back(80, config->get<int>("km_80_step"));
    conversions.emplace_back(90, config->get<int>("km_90_step"));
    conversions.emplace_back(100, config->get<int>("km_100_step"));
    conversions.emplace_back(110, config->get<int>("km_110_step"));
    conversions.emplace_back(120, config->get<int>("km_120_step"));
    conversions.emplace_back(130, config->get<int>("km_130_step"));
    conversions.emplace_back(140, config->get<int>("km_140_step"));
    conversions.emplace_back(150, config->get<int>("km_150_step"));
    conversions.emplace_back(160, config->get<int>("km_160_step"));
    conversions.emplace_back(170, config->get<int>("km_170_step"));
    conversions.emplace_back(180, config->get<int>("km_180_step"));
    conversions.emplace_back(190, config->get<int>("km_190_step"));
    conversions.emplace_back(200, config->get<int>("km_200_step"));
    conversions.emplace_back(210, config->get<int>("km_210_step"));
    conversions.emplace_back(220, config->get<int>("km_220_step"));
    conversions.emplace_back(230, config->get<int>("km_230_step"));
    conversions.emplace_back(240, config->get<int>("km_240_step"));

    loopInterval = config->get<useconds_t>("loop_interval");
    stepOffset = config->get<int>("step_offset");

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

Speedometer::~Speedometer()
{
}

void Speedometer::loop()
{
    while (!terminateFlag.load())
    {
        std::this_thread::sleep_for(std::chrono::microseconds(loopInterval));

        speed = speedSensorData->speed;

        if (speed == lastSpeed)
        {
            motor->stop();
            continue;
        }

        lastSpeed = speed;

        if (!speed || speed < 0)
            speed = 0;
        else if (speed > 240)
            speed = 240;

        stepToGo = convertToStep(speed);

        // Calculate the difference between the target and current step
        int stepDifference = std::abs(stepToGo - currentStep);

        // Dynamically adjust the motor speed based on step difference
        if (stepDifference > 100)
        {
            motor->setSpeed(4); // Fastest speed for large adjustments
        }
        else if (stepDifference > 50)
        {
            motor->setSpeed(3); // Medium-fast speed for moderate adjustments
        }
        else if (stepDifference > 10)
        {
            motor->setSpeed(2); // Medium speed for smaller adjustments
        }
        else
        {
            motor->setSpeed(1); // Slowest speed for fine adjustments
        }

        motor->step(stepToGo - currentStep);
        currentStep = stepToGo;
    }

    logger->info("Terminating process...");
    motor->step(-currentStep);
    logger->info("Done!");
}
