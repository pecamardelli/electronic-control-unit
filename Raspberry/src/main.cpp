#include "main.h"

int main(int argc, char *argv[])
{
	Logger logger("Main");
	logger.info("Program started.");

	useconds_t mainLoopInterval = sys.getConfigValue<useconds_t>("global", "main_loop_interval");

	// Vector of smart pointers to the base class
	std::vector<std::shared_ptr<Process>> processes;
	// Adding derived class objects to the vector
	processes.push_back(std::make_shared<TempGauge>());
	processes.push_back(std::make_shared<FlowSensor>());
	printf("Hola1\n");
	RoundDisplay roundDisplay;
	AnalogConverter analogConverter;
	CoolantTempSensor coolantTempSensor;
	GPS gps;
	printf("Hola2\n");

	// Exception handling: ctrl + c
	signal(SIGINT, signalHandler);

	roundDisplay.showLogo();
	sleep(2);
	roundDisplay.setScreen(DIGITAL_GAUGE);
	printf("Hola3\n");

	logger.info("Setting up shared memory for the engine readings.");
	EngineValues *engineValues = (EngineValues *)mmap(
		NULL,
		sizeof(EngineValues),
		PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANONYMOUS,
		-1,
		0);

	if (engineValues == MAP_FAILED || engineValues == NULL)
	{
		logger.error("mmap failed for engine readings or returned NULL pointer.");
	}

	logger.info("Setting up shared memory for the Flow Sensor.");
	// Create shared memory for the Flow Sensor
	FlowSensorData *flowSensorData = (FlowSensorData *)mmap(
		NULL,
		sizeof(FlowSensorData),
		PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANONYMOUS,
		-1,
		0);

	if (flowSensorData == MAP_FAILED || flowSensorData == NULL)
	{
		logger.error("mmap failed for Flow Sensor data or returned NULL pointer.");
	}

	// Iterating and calling the loop method on each process
	for (const auto &process : processes)
	{
		pid_t pid = fork();

		if (pid < 0)
		{
			logger.error("Fork failed for " + process->description);
		}
		else if (pid == 0)
		{
			logger.setDescription(process->description + "Process");
			logger.info(process->description + " child process started.");

			signal(SIGTERM, signalHandler);

			process->setup();

			while (!terminateChildProcess)
			{
				process->loop();
			}

			logger.info("Received SIGTERM signal. Cleaning up resources...");

			return 0;
		}
		else
		{
			// Track the child PID and description
			ChildProcess childProcess = {pid, process->description};
			childProcesses.push_back(childProcess);
		}
	}

	// gps.readData();

	logger.info("Entering main loop.");
	// ### MAIN LOOP ###
	while (true)
	{
		engineValues->temp = coolantTempSensor.readTemp();
		engineValues->volts = analogConverter.getVolts();
		engineValues->fuelConsumption = flowSensorData->totalConsumption;
		engineValues->kml = flowSensorData->totalPulseCount;

		roundDisplay.draw(engineValues);

		if (engineValues->volts < 6)
		{
			engineValues->ignition = false;
		}
		else
		{
			engineValues->ignition = true;
		}

		// SIGINT handler will set this flag to true.
		if (terminateProgram)
			break;

		usleep(mainLoopInterval);
	}

	logger.info("Exiting main loop. Cleaning up resources.");

	roundDisplay.setScreen(TORINO_LOGO);
	roundDisplay.showLogo();

	terminateChildProcesses(childProcesses);

	logger.info("Exiting...");
	sys.shutdown();

	logger.info("Program terminated.");
}
