#include "main.h"

int main(int argc, char *argv[])
{
	CircularDisplay display;
	display.drawBmpFile("./assets/torino_logo.bmp");
	sleep(2);
	display.clear();
	display.drawTest("78.5", 40, 60);

	while (1)
	{
		usleep(1000);
	}
}
