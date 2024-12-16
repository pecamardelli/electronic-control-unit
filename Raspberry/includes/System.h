#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include <iostream>
#include <ctime>

#include "Logger.h"
#include "../helpers/helpers.h"
#include "Relay.h"

void signal_handler(int);

class System
{
private:
    /* data */
    Relay mainRelay;
    Logger logger = Logger("System");

public:
    System(/* args */);
    ~System();
    useconds_t flowSensorLoopRate;
    useconds_t mainProgramLoopRate;

    void shutdown();
    static uint64_t uptime();
};

#endif