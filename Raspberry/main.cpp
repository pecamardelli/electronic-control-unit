#include "main.h"

int main(int argc, char *argv[])
{
	DigitalGauge digitalGauge;

	while (1)
	{

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

		if (volts < 16)
		{
			volts += 0.1;
		}
		else
		{
			volts = 8;
		}
		digitalGauge.draw(temp, kml, volts);

		usleep(50000);
	}
}
