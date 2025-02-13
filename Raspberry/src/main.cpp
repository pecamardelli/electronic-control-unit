#include "main.h"

int main(int argc, char *argv[])
{
	std::string programName = getProgramName(argv[0]);

	Logger logger("Main");
	logger.info("Program started.");

	if (!bcm2835_init())
	{
		logger.error("BCM2835 initialization failed!");
		exit(1);
	}

	bcm2835_i2c_begin();
	bcm2835_i2c_set_baudrate(1000000);

	logger.info("BCM2835 initialized!");

	sys = new System(programName);
	Config config("global");

	uint64_t mileage = sys->getCurrentMileage();
	uint64_t lastSavedMileage = 0, currentMileage = 0;

	ads1115 = std::make_unique<ADS1115>();
	useconds_t mainLoopInterval = config.get<useconds_t>("main_loop_interval");
	// unsigned int logoTime = config.get<unsigned int>("logo_screen_time");
	TCA9548A i2cMultiplexer;
	i2cMultiplexer.selectChannel(1);

	// Add smart pointer factories to the vector
	// processFactories.push_back({"TempGauge", []()
	// 							{ return std::make_shared<TempGauge>(); }});
	processFactories.push_back({"SpeedSensor", []()
								{ return std::make_shared<SpeedSensor>(); }});

	DigitalGauge digitalGauge;
	VoltSensor voltSensor(ads1115.get());
	DS18B20 coolantTempSensor;
	DHT11 tempSensor;
	Speedometer speedometer;

	digitalGauge.showLogo();
	digitalGauge.setScreen(DIGITAL_GAUGE);

	logger.info("Setting up shared memory for sensor readings.");
	engineValues = createSharedMemory<EngineValues>("/engineValues", true);
	speedSensorData = createSharedMemory<SpeedSensorData>("/speedSensorData", true);
	coolantTempSensorData = createSharedMemory<CoolantTempSensorData>("/coolantTempSensorData", true);

	// Iterate and instantiate processes during iteration
	for (const auto &factory : processFactories)
	{
		pid_t pid = fork();

		if (pid < 0)
		{
			logger.error("Fork failed!!");
		}
		else if (pid == 0)
		{
			// Instantiate here
			std::shared_ptr<Process> process = factory.create();
			process->loop();
			exit(0);
		}
		else
		{
			// Track the child PID and description
			ChildProcess childProcess = {pid, factory.typeName};
			childProcesses.push_back(childProcess);
		}
	}

	logger.info("Entering main loop.");
	// ### MAIN LOOP ###
	while (!terminateProgram)
	{
		coolantTempSensorData->temp = coolantTempSensor.readTemp();
		engineValues->volts.store(voltSensor.getValue());
		currentMileage = mileage + floor(speedSensorData->distanceCovered);

		if (currentMileage - lastSavedMileage >= 1)
		{
			sys->saveTotalMileage(currentMileage);
			lastSavedMileage = currentMileage;
			std::cout << "Mileage: " << lastSavedMileage << std::endl;
		}

		// std::cout << "Speed Sensor transitions: " << speedSensorData->transitions << std::endl;
		// std::cout << "Speed Sensor speed: " << speedSensorData->speed << std::endl;
		// std::cout << "Speed Sensor distance: " << speedSensorData->distanceCovered << std::endl;

		std::cout << "Volts: " << engineValues->volts.load() << std::endl;

		speedometer.loop();
		// i2cMultiplexer.selectChannel(1);

		digitalGauge.draw();

		// if (engineValues->volts < 6)
		// {
		// 	if (engineValues->ignition)
		// 	{
		// 		terminateProgram = true;
		// 	}
		// 	engineValues->ignition = false;
		// }
		// else
		// {
		// 	engineValues->ignition = true;
		// }

		std::this_thread::sleep_for(std::chrono::microseconds(mainLoopInterval));
		// break;
	}

	logger.info("Exiting main loop. Cleaning up resources.");

	// Cleanup shared memory spaces.
	munmap(const_cast<void *>(reinterpret_cast<const volatile void *>(engineValues)), sizeof(EngineValues));
	shm_unlink("/engineValuesMemory");
	munmap(const_cast<void *>(reinterpret_cast<const volatile void *>(speedSensorData)), sizeof(SpeedSensorData));
	shm_unlink("/speedSensorData");
	munmap(const_cast<void *>(reinterpret_cast<const volatile void *>(coolantTempSensorData)), sizeof(CoolantTempSensorData));
	shm_unlink("/coolantTempSensorData");

	digitalGauge.setScreen(TORINO_LOGO);
	digitalGauge.showLogo();

	terminateChildProcesses(childProcesses);

	bcm2835_i2c_end();
	bcm2835_close();

	logger.info("Exiting...");
	sys->shutdown();

	logger.info("Program terminated.");
}
