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

	// Setting up shared memory
	engineValues = createSharedMemory<EngineValues>("/engineValues", true);
	speedSensorData = createSharedMemory<SpeedSensorData>("/speedSensorData", true);
	coolantTempSensorData = createSharedMemory<CoolantTempSensorData>("/coolantTempSensorData", true);
	mileage = createSharedMemory<MileageData>("/mileageData", true);

	logger.info("Shared memory successfully created!");

	sys = new System(programName);
	Config config("global");
	useconds_t mainLoopInterval = config.get<useconds_t>("main_loop_interval");
	bool debugEnabled = config.get<bool>("debug_enabled");

	double lastDistanceCovered = 0;
	double lastFuelConsumption = 0;
	std::ostringstream roundedPartialMileage;

	ads1115 = std::make_unique<ADS1115>();
	VoltSensor voltSensor(ads1115.get());
	// DS3231 clock;

	// DHT11 tempSensor;

	SSD1306Hardware speedometerUpperDisplay;

	// Add smart pointer factories to the vector
	// processFactories.push_back({"MCP2515", []()
	// 							{ return std::make_shared<MCP2515>(); }});
	// processFactories.push_back({"TempGauge", []()
	// 							{ return std::make_shared<TempGauge>(); }});
	// processFactories.push_back({"DigitalGauge", []()
	// 							{ return std::make_shared<DigitalGauge>(); }});
	processFactories.push_back({"Speedometer", []()
								{ return std::make_shared<Speedometer>(); }});
	processFactories.push_back({"SpeedSensor", []()
								{ return std::make_shared<SpeedSensor>(); }});
	processFactories.push_back({"SSD1306Software", []()
								{ return std::make_shared<SSD1306Software>(); }});

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
		engineValues->volts = voltSensor.getValue();

		// mileage->currentTotal = mileage->total + floor(speedSensorData->distanceCovered);
		// mileage->currentPartial = mileage->partial + speedSensorData->distanceCovered;
		mileage->currentTotal = mileage->currentTotal + 1;
		mileage->currentPartial = mileage->currentPartial + 0.1;

		if (mileage->currentTotal - mileage->lastTotalSaved >= 1)
		{
			sys->saveMileage();
			mileage->lastTotalSaved = mileage->currentTotal;
		}

		if (mileage->currentPartial - mileage->lastPartialSaved >= 0)
		{
			roundedPartialMileage.str(""); // Clear the content
			roundedPartialMileage.clear(); // Reset error flags
			roundedPartialMileage << std::fixed << std::setprecision(1) << mileage->currentPartial;

			sys->saveMileage();
			mileage->lastPartialSaved = mileage->currentPartial;
		}

		speedometerUpperDisplay.drawString(SSD1306_ALIGN_CENTER, roundedPartialMileage.str().c_str(), LiberationSansNarrow_Bold28);
		engineValues->kml = lastFuelConsumption > 0 ? lastDistanceCovered / lastFuelConsumption : 0;
		lastDistanceCovered = speedSensorData->distanceCovered;

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

		// Check if system time and clock time are the same.
		// clock.compareTime();

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
	munmap(const_cast<void *>(reinterpret_cast<const volatile void *>(mileage)), sizeof(MileageData));
	shm_unlink("/mileageData");

	// digitalGauge.setScreen(TORINO_LOGO);
	// digitalGauge.showLogo();

	terminateChildProcesses(childProcesses);

	bcm2835_i2c_end();
	bcm2835_close();

	logger.info("Exiting...");
	sys->shutdown();

	logger.info("Program terminated.");
}
