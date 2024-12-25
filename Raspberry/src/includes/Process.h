#pragma once

#ifndef STRING_H_
#define STRING_H_
#include <string>
#endif

#include <iostream>
#include <csignal>
#include <atomic>

#include "Logger.h"
#include "Config.h"
#include "../common.h"

class Process
{
private:
    /* data */
public:
    Process();
    virtual ~Process();
    std::string description;

    // Pure virtual function
    virtual void loop(EngineValues *) = 0;

    static void handleSignal(int);

protected:
    static std::atomic<bool> terminateFlag; // Shared termination flag
    useconds_t loopInterval;
    Logger *logger;
    Config *config;
};