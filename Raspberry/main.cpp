#include "main.h"

int main(int argc, char *argv[])
{
	System sys;
	RoundDisplay roundDisplay;
	Ads1115 ads;

	while (1)
	{
		engineValues.volts = ads.getVolts();

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
