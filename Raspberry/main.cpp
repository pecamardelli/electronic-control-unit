#include "main.h"

int main(int argc, char *argv[])
{
	Logger logger("Main");
	logger.info("Program started.");

	// Exception handling: ctrl + c
	signal(SIGINT, signal_handler);

	System sys;
	RoundDisplay roundDisplay;
	AnalogConverter analogConverter;
	CoolantTempSensor coolantTempSensor;
	FlowSensor flowSensor;
	TempGauge tempGauge;

	logger.info("Initializing Round Display.");
	roundDisplay.showLogo();
	sleep(2);
	roundDisplay.setScreen(DIGITAL_GAUGE);

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

	logger.info("Creating child process for Flow Sensor.");
	logger.debug("Parent process PID: " + std::to_string(getpid()));
	logger.debug("Flow Sensor process will fork.");

	pid_t flowSensorPid = fork();

	if (flowSensorPid < 0)
	{
		logger.error("Fork failed for Flow Sensor process.");
		munmap(flowSensorData, sizeof(FlowSensorData));
	}
	else if (flowSensorPid == 0)
	{
		logger.info("Flow Sensor child process started.");
		while (1)
		{
			*flowSensorData = flowSensor.loop();
			usleep(sys.flowSensorLoopRate);
		}

		exit(0);
	}
	// Track the child PID and description
	ChildProcess flowSensorProcess = {flowSensorPid, "Flow Sensor"};
	childProcesses.push_back(flowSensorProcess);

	pid_t tempGaugePid = fork();

	if (tempGaugePid < 0)
	{
		logger.error("Fork failed for Temp Gauge process.");
	}
	else if (tempGaugePid == 0)
	{
		logger.info("Temp Gauge child process started.");
		tempGauge.setup();

		while (1)
		{
			// std::string input;
			// std::cout << "Type something and press Enter: ";
			// std::cin >> input; // Reads until the first whitespace
			// float value;

			// try
			// {
			// 	value = std::stof(input); // Convert string to float
			// 	std::cout << "Parsed float: " << value << std::endl;
			// }
			// catch (const std::invalid_argument &e)
			// {
			// 	std::cerr << "Invalid input: Not a valid float!" << std::endl;
			// 	continue;
			// }
			// catch (const std::out_of_range &e)
			// {
			// 	std::cerr << "Invalid input: Float value out of range!" << std::endl;
			// 	continue;
			// }

			tempGauge.loop(engineValues->temp);

			if (terminateChildProcess)
				break;
		}

		logger.info("Received SIGTERM signal. Cleaning up resources...");
		tempGauge.~TempGauge();
		exit(0);
	}
	// Track the child PID and description
	ChildProcess tempGauteProcess = {tempGaugePid, "Temp Gauge"};
	childProcesses.push_back(tempGauteProcess);

	logger.info("Entering main loop.");
	// ### MAIN LOOP ###
	while (1)
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

		usleep(sys.mainProgramLoopRate);
	}

	logger.info("Exiting main loop. Cleaning up resources.");

	roundDisplay.setScreen(TORINO_LOGO);
	roundDisplay.showLogo();

	terminateChildProcesses(childProcesses);

	logger.info("Exiting...");
	sys.shutdown();

	logger.info("Program terminated.");
}
