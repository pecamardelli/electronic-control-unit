#include "main.h"

int main(int argc, char *argv[])
{
	CircularDisplay display;
	display.drawBmpFile("./assets/torino_logo.bmp");
	sleep(4);
	display.clear();
	display.drawTest();

	while (1)
	{
		usleep(1000);
	}
}
