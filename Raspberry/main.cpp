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

	// Pipes to comunicate with the flow sensor process
	int flowSensorPipe[2]; // Array to hold pipe ends: pipe_fd[0] for read, pipe_fd[1] for write

	if (pipe(flowSensorPipe) == -1)
	{
		perror("Pipe creation for the Flow Sensor failed");
	}

	// Create a child process for the flow sensor
	pid_t flowSensorPid = fork();

	if (flowSensorPid < 0)
	{
		perror("Fork failed");
	}
	else if (flowSensorPid == 0)
	{
		// Child process for the Flow Sensor
		close(flowSensorPipe[0]); // Close unused read end

		while (1)
		{
			flowSensor.checkPulses();

			if (write(flowSensorPipe[1], &flowSensor.pulse_count, sizeof(flowSensor.pulse_count)) == -1)
			{
				perror("Write failed");
			}
			printf("Child pulses: %lu\n", flowSensor.pulse_count);
			usleep(5000);
		}
	}

	// close(flowSensorPipe[1]); // Close unused write end

	while (1)
	{
		engineValues.temp = coolantTempSensor.readTemp();
		engineValues.volts = analogConverter.getVolts();
		unsigned long pulses = 1;

		read(flowSensorPipe[0], &pulses, sizeof(pulses));

		printf("Pulses: %ld\n", pulses);

		engineValues.kml = (float)pulses;

		roundDisplay.draw();
		usleep(50000);
	}
}
