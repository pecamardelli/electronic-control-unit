#include "main.h"

int main(int argc, char *argv[])
{
	System sys;
	DigitalGauge digitalGauge;
	Ads1115 ads;

	while (1)
	{
		volts = ads.getVolts();

		// if (volts < 5)
		// 	sys.shutdown();

		if (temp < 125)
		{
			temp++;
		}
		else
		{
			temp = 0;
		}

		if (kml < 16)
		{
			kml += 0.1;
		}
		else
		{
			kml = 0;
		}

		digitalGauge.draw(temp, kml, volts);

		usleep(50000);
	}
}
