#include "main.h"

int main(int argc, char *argv[])
{
	System sys;
	RoundDisplay roundDisplay;
	AnalogConverter analogConverter;
	CoolantTempSensor coolantTempSensor;

	roundDisplay.showLogo();
	sleep(2);
	roundDisplay.setScreen(DIGITAL_GAUGE);

	while (1)
	{
		engineValues.temp = coolantTempSensor.readTemp();
		engineValues.volts = analogConverter.getVolts();

		if (engineValues.kml < 16)
		{
			engineValues.kml += 0.1;
		}
		else
		{
			engineValues.kml = 0;
		}

		roundDisplay.draw();

		usleep(50000);
	}
}
