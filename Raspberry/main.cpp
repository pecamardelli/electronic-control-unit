#include "main.h"

int main(int argc, char *argv[])
{
	DigitalGauge display;
	display.loop();

	while (1)
	{
		usleep(1000);
	}
}
