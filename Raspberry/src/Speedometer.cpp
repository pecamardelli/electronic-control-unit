#include "Speedometer.h"

Speedometer::Speedometer(/* args */)
{
    description = "Speedometer";

    motor = std::make_unique<Stepper>(2038, IN1, IN3, IN2, IN4);
    interruptor = std::make_unique<Button>(INTERRUPTOR);
    logger = std::make_unique<Logger>(description);
    config = std::make_unique<Config>(description);
    upperDisplay = std::make_unique<SSD1306>();

    logger->info("Setting up...");

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

    motor->setSpeed(4);
    logger->info("Ready!");
}

Speedometer::~Speedometer()
{
}

void Speedometer::loop(EngineValues *engineValues)
{
    uint64_t lastTotalMileage = 0;
    float speed = 0;
    float lastSpeed = 0;
    int stepToGo;

    while (!terminateFlag.load())
    {
        usleep(loopInterval);

        if (lastTotalMileage != engineValues->totalMileage.load())
        {
            lastTotalMileage = engineValues->totalMileage.load();
            upperDisplay->drawString(SSD1306_ALIGN_RIGHT, std::to_string(lastTotalMileage).c_str(), LiberationSansNarrow_Bold28);
        }

        speed = engineValues->speed.load();

        if (speed == lastSpeed)
        {
            continue;
        }

        lastSpeed = speed;

        if (!speed || speed < 0)
            speed = 0;
        else if (speed > 240)
            speed = 240;

        stepToGo = convertToStep(speed);

        motor->step(stepToGo - currentStep);
        motor->stop();
        currentStep = stepToGo;
    }
}