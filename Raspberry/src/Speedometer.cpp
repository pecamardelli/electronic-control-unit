#include "Speedometer.h"

Speedometer::Speedometer(/* args */)
{
    description = "Speedometer";
    logger = new Logger(description);
    config = new Config(description);

    logger->info("Setting up...");

    conversions[0].step = config->get<int>("km_20_step");
    conversions[1].step = config->get<int>("km_30_step");
    conversions[2].step = config->get<int>("km_40_step");
    conversions[3].step = config->get<int>("km_50_step");
    conversions[4].step = config->get<int>("km_60_step");
    conversions[5].step = config->get<int>("km_70_step");
    conversions[6].step = config->get<int>("km_80_step");
    conversions[7].step = config->get<int>("km_90_step");
    conversions[8].step = config->get<int>("km_100_step");
    conversions[9].step = config->get<int>("km_110_step");
    conversions[10].step = config->get<int>("km_120_step");
    conversions[11].step = config->get<int>("km_130_step");
    conversions[12].step = config->get<int>("km_140_step");
    conversions[13].step = config->get<int>("km_150_step");
    conversions[14].step = config->get<int>("km_160_step");
    conversions[15].step = config->get<int>("km_170_step");
    conversions[16].step = config->get<int>("km_180_step");
    conversions[17].step = config->get<int>("km_190_step");
    conversions[18].step = config->get<int>("km_200_step");
    conversions[19].step = config->get<int>("km_210_step");
    conversions[20].step = config->get<int>("km_220_step");
    conversions[21].step = config->get<int>("km_230_step");
    conversions[22].step = config->get<int>("km_240_step");

    loopInterval = config->get<useconds_t>("loop_interval");
    stepOffset = config->get<int>("step_offset");

    ssd1306 = new SSD1306();

    motor.setSpeed(3);
    button.check();

    // Step forward until button is released.
    while (button.pressed)
    {
        motor.step(1);
        button.check();
    }

    // Get back to the initial position.
    goToStartPosition();

    motor.setSpeed(3);

    logger->info("Speedometer ready!");
}

Speedometer::~Speedometer()
{
}

void Speedometer::loop(EngineValues *engineValues)
{
    int number = 0;
    while (!terminateFlag.load())
    {

        std::cout << "Enter an integer: ";
        std::cin >> number; // Store user input into the variable
        if (number == 0)
        {
            goToStartPosition();
        }
        else
        {
            motor.step(number);
        }
        usleep(loopInterval);
    }
}

void Speedometer::goToStartPosition()
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