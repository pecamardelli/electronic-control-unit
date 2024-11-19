#include "main.h"

int main(int argc, char *argv[])
{
	CircularDisplay display;
	display.drawImage("./assets/torino_logo.bmp");
	sleep(4);
	display.clear();

	while (1)
	{
		usleep(1000);
	}
}
