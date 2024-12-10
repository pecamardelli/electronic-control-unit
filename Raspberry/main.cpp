#include "main.h"

int main(int argc, char *argv[])
{
	System sys;
	RoundDisplay roundDisplay;
	AnalogConverter analogConverter;

	roundDisplay.showLogo();
	sleep(2);
	roundDisplay.setScreen(DIGITAL_GAUGE);

	while (1)
	{
		engineValues.volts = analogConverter.getVolts();

		// if (volts < 5)
		// 	sys.shutdown();

		if (engineValues.temp < 125)
		{
			engineValues.temp++;
		}
		else
		{
			engineValues.temp = 0;
		}

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
