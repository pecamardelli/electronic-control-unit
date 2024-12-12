#include "System.h"

System::System(/* args */)
{
    // Exception handling: ctrl + c
    signal(SIGINT, signal_handler);

    mainRelay.state(HIGH);
    flowSensorLoopRate = 1000;
    mainProgramLoopRate = 50000;
}

System::~System()
{
}

void System::shutdown()
{
    printf("Shutting down...\n");
    system("init 0");
    exit(0);
}