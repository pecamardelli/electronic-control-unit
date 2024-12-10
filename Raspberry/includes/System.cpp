#include "System.h"

System::System(/* args */)
{
    // Exception handling: ctrl + c
    signal(SIGINT, signal_handler);
    mainRelay.state(HIGH);
}

System::~System()
{
}

void System::shutdown()
{
    printf("Shutting down...\n");
    system("init 0");
    // mainRelay.state(LOW);
    exit(0);
}