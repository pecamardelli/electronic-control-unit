#include "System.h"

System::System(/* args */)
{
    mainRelay.state(HIGH);
    flowSensorLoopRate = 1000;
    mainProgramLoopRate = 50000;
}

System::~System()
{
}

void System::shutdown()
{
    logger.info("Shutting down...");
    // system("init 0");
    exit(0);
}

uint64_t System::uptime()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;
}