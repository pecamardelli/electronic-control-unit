#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdlib.h>
#include <stdio.h>
#include <csignal>
#include "../helpers/helpers.h"
#include "Relay.h"

class System
{
private:
    /* data */
    Relay mainRelay;

public:
    System(/* args */);
    ~System();

    void shutdown();
};

#endif