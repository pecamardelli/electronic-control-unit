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
			usleep(5000);
		}
	}

	// ### MAIN LOOP ###
	while (1)
	{
		engineValues.temp = coolantTempSensor.readTemp();
		engineValues.volts = analogConverter.getVolts();

		printf("Fuel: %.1f\n", flowSensorData->totalConsumption);

		// engineValues.kml = flowSensorData->totalConsumption;
		engineValues.fuelConsumption = flowSensorData->totalConsumption;

		roundDisplay.draw();
		usleep(50000);
	}
}
