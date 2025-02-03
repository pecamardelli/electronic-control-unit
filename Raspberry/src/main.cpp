#include "main.h"

int main(int argc, char *argv[])
{
	std::string programName = getProgramName(argv[0]);

	sys = new System(programName);
	Logger logger("Main");
	logger.info("Program started.");
	Config config("global");

	useconds_t mainLoopInterval = config.get<useconds_t>("main_loop_interval");
	// unsigned int logoTime = config.get<unsigned int>("logo_screen_time");
	I2CMultiplexer i2cMultiplexer;
	i2cMultiplexer.selectChannel(1);

	// Add smart pointer factories to the vector
	processFactories.push_back({"TempGauge", []()
								{ return std::make_shared<TempGauge>(); }});
	processFactories.push_back({"SpeedSensor", []()
								{ return std::make_shared<SpeedSensor>(); }});
	processFactories.push_back({"Speedometer", []()
								{ return std::make_shared<Speedometer>(); }});

	DigitalGauge digitalGauge;
	AnalogConverter analogConverter;
	CoolantTempSensor coolantTempSensor;
	TempSensor tempSensor;

	digitalGauge.showLogo();
	digitalGauge.setScreen(DIGITAL_GAUGE);

	logger.info("Setting up shared memory for the engine readings.");
	EngineValues *engineValues = createSharedMemory<EngineValues>("/engineValues", true);
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
		// engineValues->volts.store(analogConverter.getVolts());
		// std::cout << "Speed Sensor transitions: " << speedSensorData->transitions << std::endl;
		// std::cout << "Speed Sensor speed: " << speedSensorData->speed << std::endl;
		// std::cout << "Speed Sensor distance: " << speedSensorData->distanceCovered << std::endl;

		std::cout << "Volts: " << engineValues->volts.load() << std::endl;
		// i2cMultiplexer.selectChannel(1);

		digitalGauge.drawVolts(analogConverter.getVolts());

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
	munmap(engineValues, sizeof(EngineValues));
	shm_unlink("/engineValuesMemory");
	munmap(const_cast<void *>(reinterpret_cast<const volatile void *>(speedSensorData)), sizeof(SpeedSensorData));
	shm_unlink("/speedSensorData");
	munmap(const_cast<void *>(reinterpret_cast<const volatile void *>(coolantTempSensorData)), sizeof(CoolantTempSensorData));
	shm_unlink("/coolantTempSensorData");

	digitalGauge.setScreen(TORINO_LOGO);
	digitalGauge.showLogo();

	terminateChildProcesses(childProcesses);

	logger.info("Exiting...");
	sys->shutdown();

	logger.info("Program terminated.");
}
