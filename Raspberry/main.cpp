#include "main.h"

int main(int argc, char *argv[])
{
	Logger logger;
	logger.info("Program started.");

	System sys;
	RoundDisplay roundDisplay;
	AnalogConverter analogConverter;
	CoolantTempSensor coolantTempSensor;
	FlowSensor flowSensor;

	logger.info("Initializing Round Display.");
	roundDisplay.showLogo();
	sleep(2);
	roundDisplay.setScreen(DIGITAL_GAUGE);

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
		perror("mmap failed");
	}

	logger.info("Creating child process for Flow Sensor.");
	logger.debug("Parent process PID: " + std::to_string(getpid()));
	logger.debug("Flow Sensor process will fork.");

	pid_t flowSensorPid = fork();

	if (flowSensorPid < 0)
	{
		logger.error("Fork failed for Flow Sensor process.");
		perror("Fork failed");
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
	}

	logger.info("Entering main loop.");
	// ### MAIN LOOP ###
	while (1)
	{
		engineValues.temp = coolantTempSensor.readTemp();
		engineValues.volts = analogConverter.getVolts();
		engineValues.fuelConsumption = flowSensorData->totalConsumption;
		engineValues.kml = flowSensorData->totalPulseCount;

		roundDisplay.draw();

		if (engineValues.volts < 6)
		{
			logger.warning("Voltage dropped below 6V. Ignition turned off.");
			engineValues.ignition = false;
		}
		else
		{
			engineValues.ignition = true;
		}

		// SIGINT handler will set this flag to true.
		if (terminateProgram)
			break;

		usleep(sys.mainProgramLoopRate);
	}

	logger.info("Exiting main loop. Cleaning up resources.");

	roundDisplay.setScreen(TORINO_LOGO);
	roundDisplay.showLogo();

	logger.info("Terminating Flow Sensor process.");

	int status;
	kill(flowSensorPid, SIGKILL);
	waitpid(flowSensorPid, &status, 0);

	if (WIFEXITED(status))
	{
		logger.info("Flow Sensor process exited normally.");
	}
	else if (WIFSIGNALED(status))
	{
		logger.warning("Flow Sensor process killed by signal.");
	}

	logger.info("Shutting down system.");
	sys.shutdown();

	logger.info("Program terminated.");
}
