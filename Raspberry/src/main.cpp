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
	useconds_t mainLoopInterval = config.get<useconds_t>("main_loop_interval");
	// unsigned int logoTime = config.get<unsigned int>("logo_screen_time");
	bool debugEnabled = config.get<bool>("debug_enabled");

	double lastDistanceCovered = 0;
	double lastFuelConsumption = 0;

	MileageData mileage = sys->getMileage();

	ads1115 = std::make_unique<ADS1115>();
	i2cMultiplexer = std::make_unique<TCA9548A>();
	VoltSensor voltSensor(ads1115.get());
	DS18B20 coolantTempSensor;
	// DHT11 tempSensor;
	SSD1306 speedometerUpperDisplay(*i2cMultiplexer, 0);
	SSD1306 speedometerLowerDisplay(*i2cMultiplexer, 1);

	// Add smart pointer factories to the vector
	processFactories.push_back({"TempGauge", []()
								{ return std::make_shared<TempGauge>(); }});
	processFactories.push_back({"Speedometer", []()
								{ return std::make_shared<Speedometer>(); }});
	processFactories.push_back({"SpeedSensor", []()
								{ return std::make_shared<SpeedSensor>(); }});
	processFactories.push_back({"FuelConsumption", []()
								{ return std::make_shared<FuelConsumption>(); }});
	processFactories.push_back({"DigitalGauge", []()
								{ return std::make_shared<DigitalGauge>(); }});

	// Setting up shared memory
	logger.info("Setting up shared memory for child processes.");
	engineValues = createSharedMemory<EngineValues>("/engineValues", true);
	speedSensorData = createSharedMemory<SpeedSensorData>("/speedSensorData", true);
	coolantTempSensorData = createSharedMemory<CoolantTempSensorData>("/coolantTempSensorData", true);
	fuelConsumptionData = createSharedMemory<FuelConsumptionData>("/fuelConsumptionData", true);

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

	// ### MAIN LOOP ###
	logger.info("Entering main loop.");

	while (!terminateProgram)
	{
		coolantTempSensorData->temp = coolantTempSensor.readTemp();
		engineValues->volts = voltSensor.getValue();

		mileage.currentTotal = mileage.total + floor(speedSensorData->distanceCovered);
		mileage.currentPartial = mileage.partial + floor(speedSensorData->distanceCovered);

		if (mileage.currentTotal - mileage.lastTotalSaved >= 1)
		{
			mileage.total = mileage.currentTotal;
			sys->saveMileage(mileage);
			mileage.lastTotalSaved = mileage.currentTotal;
			std::cout << "Mileage: " << mileage.lastTotalSaved << std::endl;
			speedometerUpperDisplay.drawString(SSD1306_ALIGN_RIGHT, std::to_string(mileage.currentTotal).c_str(), LiberationSansNarrow_Bold28);
		}

		if (mileage.currentPartial - mileage.lastPartialSaved >= 1)
		{
			mileage.partial = mileage.currentPartial;
			sys->saveMileage(mileage);
			mileage.lastPartialSaved = mileage.currentPartial;
			std::cout << "Partial: " << mileage.lastPartialSaved << std::endl;
			// speedometerLowerDisplay.drawString(SSD1306_ALIGN_RIGHT, std::to_string(mileage.currentPartial).c_str(), LiberationSansNarrow_Bold28);
		}

		engineValues->kml = lastFuelConsumption > 0 ? lastDistanceCovered / lastFuelConsumption : 0;
		lastDistanceCovered = speedSensorData->distanceCovered;
		lastFuelConsumption = fuelConsumptionData->fuelConsumption;

		if (debugEnabled)
		{
			std::cout << "Transitions: " << speedSensorData->transitions;
			std::cout << " | Speed: " << speedSensorData->speed;
			std::cout << " | Distance covered: " << speedSensorData->distanceCovered;
			std::cout << " | Volts: " << engineValues->volts << std::endl;
		}

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
	munmap(const_cast<void *>(reinterpret_cast<const volatile void *>(fuelConsumptionData)), sizeof(FuelConsumptionData));
	shm_unlink("/fuelConsumptionData");

	// digitalGauge.setScreen(TORINO_LOGO);
	// digitalGauge.showLogo();

	terminateChildProcesses(childProcesses);

	bcm2835_i2c_end();
	bcm2835_close();

	logger.info("Exiting...");
	sys->shutdown();

	logger.info("Program terminated.");
}
