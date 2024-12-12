#include "main.h"

int main(int argc, char *argv[])
{
	System sys;
	RoundDisplay roundDisplay;
	AnalogConverter analogConverter;
	CoolantTempSensor coolantTempSensor;
	FlowSensor flowSensor;

	roundDisplay.showLogo();
	sleep(2);
	roundDisplay.setScreen(DIGITAL_GAUGE);

	// Create shared memory for the Flow Sensor
	FlowSensorData *flowSensorData = (FlowSensorData *)mmap(
		NULL,
		sizeof(FlowSensorData),
		PROT_READ | PROT_WRITE,
		MAP_SHARED | MAP_ANONYMOUS,
		-1,
		0);

	if (flowSensorData == MAP_FAILED)
	{
		perror("mmap failed");
	}

	// Create a child process for the Flow Sensor
	pid_t flowSensorPid = fork();

	if (flowSensorPid < 0)
	{
		perror("Fork failed");
		munmap(flowSensorData, sizeof(FlowSensorData));
	}
	else if (flowSensorPid == 0)
	{
		while (1)
		{
			*flowSensorData = flowSensor.loop();
			usleep(sys.flowSensorLoopRate);
		}
	}

	// ### MAIN LOOP ###
	while (1)
	{
		engineValues.temp = coolantTempSensor.readTemp();
		engineValues.volts = analogConverter.getVolts();
		engineValues.fuelConsumption = flowSensorData->totalConsumption;
		engineValues.kml = flowSensorData->totalConsumption;
		roundDisplay.draw();

		// SIGINT handler will set this flag to true.
		if (terminateProgram)
			break;

		usleep(sys.mainProgramLoopRate);
	}

	roundDisplay.setScreen(TORINO_LOGO);
	roundDisplay.showLogo();

	printf("Terminating Flow Sensor process...\n");

	int status;

	kill(flowSensorPid, SIGKILL);
	waitpid(flowSensorPid, &status, 0);

	if (WIFEXITED(status))
	{
		printf("Flow Sensor process exited normally with status %d\n", WEXITSTATUS(status));
	}
	else if (WIFSIGNALED(status))
	{
		printf("Flow Sensor process killed by signal %d\n", WTERMSIG(status));
	}

	sys.shutdown();
}
